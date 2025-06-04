//
// Created by TheDaChicken on 6/15/2024.
//

#include "HybridRadio.h"

#include <memory>
#include <unordered_set>

#include "utils/Log.h"
#include "nrsc5/Decoder.h"
#include "audio/PortAudioCpp.h"
#include "audio/stream/StreamParameters.h"
#include "utils/Error.h"
#include "utils/Time.h"

#define HYBRID_RADIO_OUTPUT_FRAMES_PER (1204)
#define HYBRID_RADIO_OUTPUT_BUFFER_LENGTH (1204)
#define HYBRID_RADIO_OUTPUT_DELAY_FRAMES (size_t)(1942 * 2)

HybridRadio::HybridRadio(Delegate *delegate)
	: delegate_(delegate)
{
	assert(this->delegate_);

	int ret = nrsc5_decoder_.OpenPipe();
	// This won't happen. The function always returns 0.
	if (ret < 0)
		throw std::runtime_error("Failed to open NRSC5 pipe");

	nrsc5_decoder_.SetCallback(NRSC5Callback, this);
}

UTILS::StatusCodes HybridRadio::Start()
{
	if (!sdr_stream_)
		return UTILS::StatusCodes::UnknownError;

	int ret = sdr_stream_->Start();
	if (ret < 0)
	{
		Logger::Log(err, "Failed to start SDR stream");
		return UTILS::StatusCodes::TunerError;
	}

	// Start audio output
	if (!audio_disabled)
	{
		ret = audio_stream_.Start();
		if (ret < 0)
		{
			sdr_stream_->Stop();

			Logger::Log(err, "Failed to start audio output");
			return UTILS::StatusCodes::UnknownError;
		}
	}

	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes HybridRadio::Stop()
{
	if (!sdr_stream_)
		return UTILS::StatusCodes::UnknownError;

	int ret = sdr_stream_->Stop();
	if (ret < 0)
	{
		Logger::Log(err, "Failed to stop SDR stream");
		return UTILS::StatusCodes::TunerError;
	}

	// Reset libnrsc5
	nrsc5_decoder_.SetFrequency(-1);

	if (!audio_disabled)
	{
		// Stop audio output
		ret = audio_stream_.Stop();
		if (ret < 0)
		{
			Logger::Log(err, "Failed to stop audio output");
			return UTILS::StatusCodes::UnknownError;
		}
	}

	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes HybridRadio::ClearSDRDevice()
{
	if (!sdr_stream_)
		return UTILS::StatusCodes::Ok;

	if (IsAudioActive())
	{
		Stop();
	}

	sdr_stream_.reset();
	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes HybridRadio::SetSDRDevice(
	const std::shared_ptr<PortSDR::Device> &device)
{
	if (!device)
		return UTILS::StatusCodes::UnknownError;

	int ret = device->CreateStream(sdr_stream_);
	if (ret < 0)
	{
		Logger::Log(warn, "Failed to create SDR stream");
		return UTILS::StatusCodes::TunerError;
	}

	sdr_stream_->SetCallback([this](const void *data, const std::size_t elementSize)
	{
		SDRCallback(static_cast<const int16_t *>(data), elementSize);
	});

	// TODO dynamic sample rate
	ret = sdr_stream_->SetSampleRate(NRSC5_SAMPLE_RATE_CU8);
	if (ret < 0)
	{
		Logger::Log(err, "Failed to set SDR sample rate {}", ret);
		return UTILS::StatusCodes::TunerError;
	}

	sdr_stream_->SetSampleFormat(PortSDR::SAMPLE_FORMAT_INT16);
	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes HybridRadio::SetAudioDevice(
	const std::shared_ptr<PortAudio::Device> &device)
{
	if (!device)
		return UTILS::StatusCodes::UnknownError;

	if (device->isNull())
	{
		Logger::Log(warn, "Selected Device: Disabled");
		audio_disabled = true;
		return UTILS::StatusCodes::Ok;
	}

	PortAudio::StreamParametersRingBuffer params;

	params.outputParameters_.SetDevice(device);
	params.outputParameters_.channelCount = 2;
	params.outputParameters_.suggestedLatency = device->DefaultLowOutputLatency();
	params.outputParameters_.sampleFormat = PortAudio::FLOAT32;

	params.framesPerBuffer_ = HYBRID_RADIO_OUTPUT_FRAMES_PER;
	params.bufferSize = HYBRID_RADIO_OUTPUT_BUFFER_LENGTH;
	// TODO: Maybe make this a user controllable setting
	params.bufferLatency = HYBRID_RADIO_OUTPUT_DELAY_FRAMES;
	params.sampleRate = NRSC5_SAMPLE_RATE_AUDIO;

	if (int ret = audio_stream_.Open(params); ret < 0)
	{
		Logger::Log(err, "Failed to open audio output: {}", Pa_GetErrorText(ret));
		return UTILS::StatusCodes::UnknownError;
	}

	delegate_->SetAudioStream(audio_stream_);
	audio_disabled = false;
	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes HybridRadio::SetChannel(
	const Modulation::Type type,
	const double frequency,
	unsigned int programId)
{
	if (!this->sdr_stream_)
		return UTILS::StatusCodes::UnknownError;

	return SetChannel({TunerOpts{type, frequency}, programId});
}

UTILS::StatusCodes HybridRadio::SetChannel(
	const Channel &channel)
{
	if (!this->sdr_stream_)
		return UTILS::StatusCodes::UnknownError;

	if (const UTILS::StatusCodes ret = SetTunerConfiguration(channel.tuner_opts);
		ret == UTILS::StatusCodes::Ok)
	{
		audio_stream_.Reset();
		station_info_ = channel.station_info;
	}
	else if (ret != UTILS::StatusCodes::Empty)
	{
		return ret;
	}

	SetProgram(channel.station_info.current_program);
	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes HybridRadio::SetTunerConfiguration(
	const TunerOpts &tunerOpts)
{
	if (!this->sdr_stream_)
		return UTILS::StatusCodes::UnknownError;

	if (sdr_stream_->GetCenterFrequency() == tunerOpts.freq)
		return UTILS::StatusCodes::Empty;

	if (int ret = sdr_stream_->SetCenterFrequency(tunerOpts.freq, 0);
		ret < 0)
	{
		Logger::Log(err, "Failed to set SDR frequency {}: {}", ret, tunerOpts.freq);
		return UTILS::StatusCodes::TunerError;
	}

	if (int ret = nrsc5_decoder_.SetFrequency(static_cast<float>(tunerOpts.freq));
		ret < 0)
	{
		Logger::Log(err, "Failed to reset NRSC5 Decoder {}", ret);
		return UTILS::StatusCodes::UnknownError;
	}

	{
		std::lock_guard lock(station_mutex_);

		station_info_.Reset();
		station_details_.Reset();
		ber_ = {};
	}

	Logger::Log(debug, "HybridRadio: frequency has been set: {} hz", tunerOpts.freq);
	return UTILS::StatusCodes::Ok;
}

void HybridRadio::SetProgram(
	const unsigned int programId)
{
	std::lock_guard lock(station_mutex_);

	if (station_info_.current_program != programId)
	{
		station_info_.current_program = programId;
		audio_stream_.Reset();
	}

	delegate_->RadioStationUpdate(CreateChannel());
}

ActiveChannel HybridRadio::GetChannel() const
{
	if (!sdr_stream_)
		return {};

	std::lock_guard lock(station_mutex_);
	return CreateChannel();
}

void HybridRadio::SDRCallback(
	const int16_t *data,
	const std::size_t frame_size)
{
	assert(frame_size % 4 == 0);

	convert_buffer_.resize(frame_size / 4);
	int len = 0;

	for (int i = 0; i < frame_size; i += 4)
	{
		cint16_t x[2];

		x[0] = std::complex(data[i], data[i + 1]);
		x[1] = std::complex(data[i + 2], data[i + 3]);

		halfband_16_.execute(x, &convert_buffer_[len++]);
	}

	std::lock_guard lock(station_mutex_);
	nrsc5_decoder_.SendIQ(reinterpret_cast<int16_t *>(convert_buffer_.data()), len * 2);
}

void HybridRadio::NRSC5Audio(const int16_t *data, const size_t frame_size)
{
	audio_buffer_.resize(frame_size);

	for (int i = 0; i < frame_size; i++)
	{
		/* convert to float from int16_t */
		audio_buffer_[i] = static_cast<float>(data[i]) * (1.0f / 32768.0f);
	}

	//Logger::Log(debug, "HDRadio: Audio frame size={}", frame_size);

	audio_stream_.WriteFrame(audio_buffer_.data(), frame_size / 2);
}

ActiveChannel HybridRadio::CreateChannel() const
{
	assert(this->sdr_stream_);

	return {
		TunerOpts{Modulation::Type::MOD_FM, sdr_stream_->GetCenterFrequency()},
		station_info_,
		station_details_
	};
}

void HybridRadio::NRSC5Callback(const nrsc5_event_t *evt, void *opaque)
{
	auto *stream = static_cast<HybridRadio *>(opaque);

	switch (evt->event)
	{
		case NRSC5_EVENT_SYNC:
		{
			Logger::Log(info, "HDRadio: Synchronized");
			stream->delegate_->HDSyncUpdate(true);
			stream->m_sync_ = std::chrono::steady_clock::now();
			break;
		}
		case NRSC5_EVENT_LOST_SYNC:
		{
			Logger::Log(info, "HDRadio: Lost Sync");
			stream->delegate_->HDSyncUpdate(false);
			stream->m_sync_.reset();
			break;
		}
		case NRSC5_EVENT_MER:
		{
			Logger::Log(info, "HDRadio: MER: {:.1f} dB (lower), {:.1f} dB (upper)", evt->mer.lower, evt->mer.upper);
			stream->delegate_->HDSignalStrengthUpdate(evt->mer.lower, evt->mer.upper);
			break;
		}
		case NRSC5_EVENT_BER:
		{
			const float cber = evt->ber.cber;

			stream->ber_.Add(cber);

			Logger::Log(info,
			            "HDRadio: BER: {:.6f}, avg: {:.6f}, min: {:.6f}, max: {:.6f}",
			            cber,
			            stream->ber_.ber,
			            stream->ber_.min,
			            stream->ber_.max);
			break;
		}
		/* sis as a hint for programs */
		case NRSC5_EVENT_AUDIO_SERVICE_DESCRIPTOR:
		{
			const unsigned int kProgramId = evt->asd.program;

			NRSC5::Program &program = stream->station_details_.programs[kProgramId];
			program.type = evt->asd.type;

			Logger::Log(info,
			            "HD{}: Audio Service type={}",
			            NRSC5::FriendlyProgramId(kProgramId),
			            NRSC5::Decoder::ProgramTypeName(program.type));
			stream->delegate_->RadioStationUpdate(stream->CreateChannel());
			break;
		}
		/* all existing programs get called here */
		case NRSC5_EVENT_AUDIO_SERVICE:
		{
			const unsigned int kProgramId = evt->audio_service.program;

			NRSC5::Program &program = stream->station_details_.programs[kProgramId];
			program.type = evt->audio_service.type;

			Logger::Log(info,
			            "HD{}: Audio Service type={}",
			            NRSC5::FriendlyProgramId(kProgramId),
			            NRSC5::Decoder::ProgramTypeName(evt->audio_service.type));

			stream->delegate_->RadioStationUpdate(stream->CreateChannel());
			break;
		}
		case NRSC5_EVENT_DATA_SERVICE_DESCRIPTOR:
		{
			Logger::Log(info,
			            "HDRadio: Data Service access={} type={}",
			            evt->dsd.access == NRSC5_ACCESS_PUBLIC ? "public" : "restricted",
			            NRSC5::Decoder::ServiceTypeName(evt->dsd.type));
			break;
		}
		case NRSC5_EVENT_STATION_ID:
		{
			stream->station_info_.country_code = evt->station_id.country_code;
			stream->station_info_.id = evt->station_id.fcc_facility_id;

			Logger::Log(debug,
			            "HDRadio: Station ID: {} ({})",
			            stream->station_info_.id,
			            stream->station_info_.country_code);

			stream->delegate_->RadioStationUpdate(stream->CreateChannel());
			break;
		}
		case NRSC5_EVENT_STATION_NAME:
		{
			stream->station_info_.name = evt->station_name.name;

			Logger::Log(debug, "HDRadio: Station Name: {}", stream->station_info_.name);

			stream->delegate_->RadioStationUpdate(stream->CreateChannel());
			break;
		}
		case NRSC5_EVENT_STATION_MESSAGE:
		{
			stream->station_details_.message = evt->station_message.message;

			Logger::Log(debug, "HDRadio: Station Message: {}", stream->station_details_.message);
			break;
		}
		case NRSC5_EVENT_STATION_SLOGAN:
		{
			stream->station_details_.slogan = evt->station_slogan.slogan;

			Logger::Log(debug, "HDRadio: Station Slogan: {}", stream->station_details_.slogan);
			break;
		}
		/* Part of AAS (SIG = Station Information Guide) */
		case NRSC5_EVENT_SIG:
		{
			const nrsc5_sig_component_t *sig_component;
			const nrsc5_sig_service_t *sig_service;

			for (sig_service = evt->sig.services;
			     sig_service != nullptr; sig_service = sig_service->next)
			{
				Logger::Log(debug,
				            "HDRadio: SIG Service type={} number={} name={}",
				            sig_service->type == NRSC5_SIG_SERVICE_AUDIO ? "audio" : "data",
				            sig_service->number,
				            sig_service->name);

				for (sig_component = sig_service->components;
				     sig_component != nullptr; sig_component = sig_component->next)
				{
					if (sig_component->type == NRSC5_SIG_SERVICE_AUDIO)
					{
						/* data service is associated with a program */
						NRSC5::Program &program = stream->station_details_.programs[sig_component->audio.port];

						program.name = sig_service->name;

						Logger::Log(debug,
						            "  Audio component: {} port={} channel={}",
						            sig_component->id,
						            sig_component->audio.port,
						            NRSC5::FriendlyProgramId(sig_component->audio.port));
					}
					else if (sig_component->type == NRSC5_SIG_SERVICE_DATA)
					{
						Logger::Log(debug,
						            "  Data component: {} port={} mime={}",
						            sig_component->id,
						            sig_component->data.port,
						            NRSC5::DescribeMime(sig_component->data.mime));
					}
				}
			}
			break;
		}
		case NRSC5_EVENT_STREAM:
		{
			NRSC5::DataService kComponent(evt->packet.service, evt->packet.component);

			Logger::Log(info,
			            "HD{}: Stream: port={} size={} seq={} mime={} service={}",
			            kComponent.programId.has_value()
				            ? fmt::to_string(NRSC5::FriendlyProgramId(kComponent.programId.value()))
				            : "Radio",
			            evt->stream.port,
			            evt->stream.size,
			            evt->stream.seq,
			            NRSC5::DescribeMime(evt->stream.mime),
			            NRSC5::DescribeMime(kComponent.mime));
			break;
		}
		case NRSC5_EVENT_PACKET:
		{
			NRSC5::DataService kComponent(evt->packet.service, evt->packet.component);

			Logger::Log(info,
			            "HD{}: Packet: port={} size={} seq={} mime={} service={}",
			            kComponent.programId.has_value()
				            ? fmt::to_string(NRSC5::FriendlyProgramId(kComponent.programId.value()))
				            : "Radio",
			            evt->packet.port,
			            evt->packet.size,
			            evt->packet.seq,
			            NRSC5::DescribeMime(evt->packet.mime),
			            NRSC5::DescribeMime(kComponent.mime));
			break;
		}
		case NRSC5_EVENT_LOT:
		{
			NRSC5::Lot lot(evt);

			Logger::Log(info,
			            "HD{}: LOT: file port={} id={} name={} size={} mime={} service={} expire={:%Y-%m-%dT%H:%M:%SZ} (in {})",
			            lot.component.programId.has_value()
				            ? fmt::to_string(NRSC5::FriendlyProgramId(lot.component.programId.value()))
				            : "Radio",
			            lot.component.port,
			            lot.id,
			            lot.name,
			            lot.data.size(),
			            /* mime */
			            NRSC5::DescribeMime(lot.mime),
			            NRSC5::DescribeMime(lot.component.mime),
			            /* discard info */
			            lot.discard_utc,
			            std::chrono::duration_cast<std::chrono::seconds>(
				            lot.expire_point - std::chrono::system_clock::now())
			);

			stream->delegate_->HDReceivedLot(stream->station_info_, lot);
			break;
		}
		case NRSC5_EVENT_HDC:
		{
			if (evt->hdc.program == stream->station_info_.current_program)
			{
				stream->audio_packets++;
				stream->audio_bytes += evt->hdc.count * sizeof(evt->hdc.data[0]);

				if (stream->audio_packets >= 32)
				{
					Logger::Log(info,
					            "Audio bit rate: {:.1f} kbps",
					            static_cast<float>(stream->audio_bytes)
					            * 8 * NRSC5_SAMPLE_RATE_AUDIO / NRSC5_AUDIO_FRAME_SAMPLES /
					            static_cast<float>(stream->audio_packets) / 1000);
					stream->audio_packets = 0;
					stream->audio_bytes = 0;
				}
			}
			break;
		}
		case NRSC5_EVENT_ID3:
		{
			if (evt->hdc.program == stream->station_info_.current_program)
			{
				const NRSC5::ID3 kId3(evt);
				nrsc5_id3_comment_t *kComment;
				const unsigned int friendlyId = NRSC5::FriendlyProgramId(evt->id3.program);

				if (!kId3.artist.empty() || !kId3.title.empty())
					Logger::Log(info,
					            "HD{}: Artist={} Title={}",
					            friendlyId,
					            kId3.artist,
					            kId3.title);

				if (!kId3.album.empty())
					Logger::Log(info,
					            "HD{}: Album={}",
					            friendlyId,
					            kId3.album);

				if (!kId3.genre.empty())
					Logger::Log(info,
					            "HD{}: Genre={}",
					            friendlyId,
					            kId3.genre);

				if (!kId3.xhdr.Empty())
					Logger::Log(info,
					            "HD{}: XHDR: {} Lot={} mime={}",
					            friendlyId,
					            kId3.xhdr.ParamName(),
					            kId3.xhdr.lot,
					            NRSC5::DescribeMime(kId3.xhdr.mime));

				for (kComment = evt->id3.comments; kComment != nullptr; kComment = kComment->next)
					Logger::Log(info,
					            "HD{}: Comment: Lang={} {} {}",
					            friendlyId,
					            kComment->lang,
					            kComment->short_content_desc,
					            kComment->full_text);

				stream->delegate_->HDID3Update(kId3);
			}
			break;
		}
		case NRSC5_EVENT_AUDIO:
		{
			if (evt->audio.program != stream->station_info_.current_program)
				break;

			stream->NRSC5Audio(evt->audio.data, evt->audio.count);
			break;
		}
		default:
			break;
	}
}

