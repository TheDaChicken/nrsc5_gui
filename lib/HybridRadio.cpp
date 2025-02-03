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

HybridRadio::HybridRadio(Delegate *delegate) : delegate_(delegate)
{
	assert(this->delegate_);

	int ret = nrsc5_decoder_.OpenPipe();

	if (ret < 0) // This won't happen. The function always returns 0.
		throw std::runtime_error("Failed to open NRSC5 pipe");

	nrsc5_decoder_.SetCallback(NRSC5Callback, this);
}

int HybridRadio::SetSDRDevice(const std::shared_ptr<PortSDR::Device> &device)
{
	if (!device)
		return NRSC5_DEFAULT_ERROR;

	if (sdr_stream_)
	{
		Stop();
		sdr_stream_.reset();
	}

	int ret = device->CreateStream(sdr_stream_);
	if (ret < 0)
	{
		Logger::Log(warn, "Failed to create SDR stream");
		return ret;
	}

	sdr_stream_->SetCallback([this](const void* data, const std::size_t elementSize)
	{
		SDRCallback(static_cast<const int16_t *>(data), elementSize);
	});

	// TODO dynamic sample rate
	ret = sdr_stream_->SetSampleRate(NRSC5_SAMPLE_RATE_CU8);
	if (ret < 0)
	{
		Logger::Log(err, "Failed to set SDR sample rate {}", ret);
		return ret;
	}

	sdr_stream_->SetSampleFormat(PortSDR::SAMPLE_FORMAT_INT16);
	return 0;
}

int HybridRadio::SetAudioDevice(const std::shared_ptr<PortAudio::Device> &device)
{
	if (!device)
		return NRSC5_DEFAULT_ERROR;

	if (device->isNull())
	{
		Logger::Log(warn, "Selected Device: Disabled");
		audio_disabled = true;
		return 0;
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

	const int ret = audio_stream_.Open(params);
	if (ret < 0)
	{
		Logger::Log(err, "Failed to open audio output: {}", Pa_GetErrorText(ret));
		return ret;
	}

	delegate_->SetAudioStream(audio_stream_);
	audio_disabled = false;
	return 0;
}

int HybridRadio::Start()
{
	if (!sdr_stream_)
		return NRSC5_DEFAULT_ERROR;

	int ret = sdr_stream_->Start();
	if (ret < 0)
	{
		Logger::Log(err, "Failed to start SDR stream");
		return ret;
	}

	// Start audio output
	if (!audio_disabled)
	{
		ret = audio_stream_.Start();
		if (ret < 0)
		{
			Logger::Log(err, "Failed to start audio output");
			sdr_stream_->Stop();
			return ret;
		}
	}
	return 0;
}

int HybridRadio::Stop()
{
	if (!sdr_stream_)
		return NRSC5_DEFAULT_ERROR;

	int ret = sdr_stream_->Stop();
	if (ret < 0)
	{
		Logger::Log(err, "Failed to stop SDR stream");
		return ret;
	}

	// Reset libnrsc5
	nrsc5_decoder_.SetFrequency(-1);

	// Stop audio output
	ret = audio_stream_.Stop();
	if (ret < 0)
	{
		Logger::Log(err, "Failed to stop audio output");
		return ret;
	}
	return 0;
}

int HybridRadio::SetChannel(Modulation::Type type, double frequency, unsigned int programId)
{
	if (!this->sdr_stream_)
		return NRSC5_DEFAULT_ERROR;

	return SetRadioChannel({type, frequency, programId});
}

int HybridRadio::SetRadioChannel(const RadioChannel &channel)
{
	if (!this->sdr_stream_)
		return NRSC5_DEFAULT_ERROR;

	int ret = SetTunerData(channel.tuner_options);
	if (ret < 0)
	{
		Logger::Log(err, "Failed to set tuner data");
		return NRSC5_DEFAULT_ERROR;
	}

	std::lock_guard lock(station_mutex_);

	// Reset NRSC5::Station when frequency changes to avoid wrong information
	// Set station information to HD station data if we have data available
	if (ret != 1)
	{
		station_.Reset();
		station_.name = channel.hd_station_.name;
	}

	station_.current_program = channel.hd_station_.current_program;
	audio_stream_.Reset();

	delegate_->RadioStationUpdate(CreateChannel());
	return 0;
}

int HybridRadio::SetTunerData(const TunerOptions &tunerData)
{
	if (!this->sdr_stream_)
		return NRSC5_DEFAULT_ERROR;

	if (sdr_stream_->GetCenterFrequency() == tunerData.frequency_)
	{
		return 1;
	}

	int ret = sdr_stream_->SetCenterFrequency(tunerData.frequency_, 0);
	if (ret < 0)
	{
		Logger::Log(err, "Failed to set SDR frequency {}: {}", ret, tunerData.frequency_);
		return ret;
	}

	// Reset libnrsc5 decoder when frequency changes
	ret = nrsc5_decoder_.SetFrequency(static_cast<float>(tunerData.frequency_));
	if (ret < 0)
	{
		Logger::Log(err, "Failed to reset NRSC5 Decoder {}", ret);
		return ret;
	}

	Logger::Log(debug, "HybridRadio: frequency has been set: {} Hz", tunerData.frequency_);
	return 0;
}

void HybridRadio::SetProgram(const unsigned int programId)
{
	std::lock_guard lock(station_mutex_);

	station_.current_program = programId;
	delegate_->RadioStationUpdate(CreateChannel());
}

RadioChannel HybridRadio::GetChannel() const
{
	if (!sdr_stream_)
		return {};

	std::lock_guard lock(station_mutex_);
	return CreateChannel();
}

void HybridRadio::SDRCallback(const int16_t *data, std::size_t frame_size)
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

	// Lock mutex for nrsc5 decoder processing (thread safety)
	std::lock_guard lock(station_mutex_);
	// Send samples to nrsc5 decoder for processing
	nrsc5_decoder_.SendIQ(reinterpret_cast<int16_t *>(convert_buffer_.data()), len * 2);

	fflush(stderr); // This is temporary to fix nrsc5 debug library logging with spdlog.
}

void HybridRadio::NRSC5Audio(const int16_t *data, size_t frame_size)
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

void HybridRadio::NRSC5Callback(const nrsc5_event_t *evt, void *opaque)
{
	auto *stream = static_cast<HybridRadio *>(opaque);
	auto &[id,
		country_code,
		name,
		message,
		slogan,
		programId,
		programs,
		services
	] = stream->station_;

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
		case NRSC5_EVENT_SIS:
		{
			const nrsc5_sis_asd_t *audio_service;

			if (evt->sis.country_code)
			{
				country_code = evt->sis.country_code;
				id = evt->sis.fcc_facility_id;
			}
			else
			{
				country_code.clear();
				id = 0;
			}

			if (evt->sis.name)
				name = evt->sis.name;
			else
				name.clear();

			if (evt->sis.slogan)
				slogan = evt->sis.slogan;
			else
				slogan.clear();

			if (evt->sis.message)
				message = evt->sis.message;
			else
				message.clear();

			Logger::Log(info,
			            "HDRadio: Country={} ID={} Name={} Message={} Slogan={}",
			            country_code,
			            id,
			            name,
			            message,
			            slogan);

			// Clear programs (SIS is updated)
			programs.clear();

			for (audio_service = evt->sis.audio_services;
			     audio_service != nullptr; audio_service = audio_service->next)
			{
				const unsigned int kProgramId = audio_service->program;
				NRSC5::Program program;

				program.id = kProgramId;
				program.type = audio_service->type;

				Logger::Log(info,
				            "HD{}: Audio Service type={}",
				            NRSC5::FriendlyProgramId(program.id),
				            NRSC5::Decoder::ProgramTypeName(audio_service->type));

				programs.insert({kProgramId, program});
			}

			stream->delegate_->RadioStationUpdate(stream->CreateChannel());
			break;
		}
		case NRSC5_EVENT_SIG: /* Part of AAS (SIG = Station Information Guide) */
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

				std::optional<unsigned int> serviceProgram;

				for (sig_component = sig_service->components;
				     sig_component != nullptr; sig_component = sig_component->next)
				{
					if (sig_component->type == NRSC5_SIG_SERVICE_AUDIO)
					{
						/* data service is associated with a program */
						NRSC5::Program &program = programs[sig_component->audio.port];

						// Some stations are configured improperly.
						// They provide multi-programs in SIS. only one in SIG
						// This is a workaround to fix the issue.
						program.name = sig_service->name;
						program.id = sig_component->audio.port;

						Logger::Log(debug,
						            "  Audio component: {} port={} channel={}",
						            sig_component->id,
						            sig_component->audio.port,
						            NRSC5::FriendlyProgramId(program.id));

						serviceProgram = program.id;
					}
					else if (sig_component->type == NRSC5_SIG_SERVICE_DATA)
					{
						NRSC5::DataService service;
						service.program = serviceProgram;
						service.port = sig_component->data.port;
						service.type = sig_component->data.type;
						service.mime = sig_component->data.mime;

						Logger::Log(debug,
						            "  Data component: {} port={} mime={}",
						            sig_component->id,
						            service.port,
						            NRSC5::DescribeMime(service.mime));

						services.insert({service.port, service});
					}
				}
			}
			break;
		}
		case NRSC5_EVENT_STREAM:
		{
			const auto kIt = services.find(evt->stream.port);
			if (kIt == services.end())
			{
				Logger::Log(err, "HDRadio: Stream: Unknown port={}", evt->stream.port);
				break;
			}
			const NRSC5::DataService &kComponent = kIt->second;

			if (kComponent.type != 0)
			{
				Logger::Log(warn, "Wrong component type {}. port={}", kComponent.type, kComponent.port);
			}

			Logger::Log(info,
			            "HD{}: Stream: port={} size={} seq={} mime={} service={}",
			            kComponent.program.has_value()
				            ? fmt::to_string(NRSC5::FriendlyProgramId(kComponent.program.value()))
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
			const auto kIt = services.find(evt->packet.port);
			if (kIt == services.end())
			{
				Logger::Log(err, "HDRadio: Packet: Unknown port={}", evt->packet.port);
				break;
			}
			const NRSC5::DataService &kComponent = kIt->second;

			if (kComponent.type != 1)
			{
				Logger::Log(warn, "Wrong component type {}. port={}", kComponent.type, kComponent.port);
			}

			Logger::Log(info,
			            "HD{}: Packet: port={} size={} seq={} mime={} service={}",
			            kComponent.program.has_value()
				            ? fmt::to_string(NRSC5::FriendlyProgramId(kComponent.program.value()))
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
			const auto kIt = services.find(evt->lot.port);
			if (kIt == services.end())
			{
				Logger::Log(err, "HDRadio: LOT: Unknown port={}", evt->lot.port);
				break;
			}

			const NRSC5::DataService &kComponent = kIt->second;
			const NRSC5::Lot lot(evt);

			Logger::Log(info,
			            "HD{}: LOT: file port={} id={} name={} size={} mime={} service={} expire={:%Y-%m-%dT%H:%M:%SZ} (in {})",
			            kComponent.program.has_value()
				            ? fmt::to_string(NRSC5::FriendlyProgramId(kComponent.program.value()))
				            : "Radio",
			            lot.port,
			            lot.id,
			            lot.name,
			            lot.data.size(),
			            /* mime */
			            NRSC5::DescribeMime(lot.mime),
			            NRSC5::DescribeMime(kComponent.mime),
			            /* discard */
			            lot.discard_utc,
			            std::chrono::duration_cast<std::chrono::seconds>(
				            lot.expire_point - std::chrono::system_clock::now())
			);

			stream->delegate_->HDReceivedLot(stream->CreateChannel(), kComponent, lot);
			break;
		}
		case NRSC5_EVENT_HDC:
		{
			if (evt->hdc.program == stream->station_.current_program)
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
			if (evt->hdc.program == stream->station_.current_program)
			{
				const NRSC5::ID3 kId3(evt);
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

				stream->delegate_->HDID3Update(kId3);
			}
			break;
		}
		case NRSC5_EVENT_AUDIO:
		{
			if (evt->audio.program != stream->station_.current_program)
				break;

			stream->NRSC5Audio(evt->audio.data, evt->audio.count);
			break;
		}
		default:
			break;
	}
}

RadioChannel HybridRadio::CreateChannel() const
{
	assert(this->sdr_stream_);

	return {
		TunerOptions{Modulation::Type::MOD_FM, sdr_stream_->GetCenterFrequency()},
		station_
	};
}
