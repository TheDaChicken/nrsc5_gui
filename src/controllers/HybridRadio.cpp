//
// Created by TheDaChicken on 6/15/2024.
//

#include "HybridRadio.h"

#include <memory>
#include <qtconcurrentrun.h>
#include <unordered_set>

#include "Application.h"
#include "utils/Log.h"
#include "nrsc5/Decoder.h"
#include "audio/PortAudioCpp.h"
#include "audio/stream/StreamParameters.h"
#include "utils/Error.h"
#include "utils/Time.h"

#define HYBRID_RADIO_OUTPUT_FRAMES_PER (1204)
#define HYBRID_RADIO_OUTPUT_BUFFER_LENGTH (1204)
#define HYBRID_RADIO_OUTPUT_DELAY_FRAMES (size_t)(1942 * 2)

HybridRadio::HybridRadio(SQLite::Database &db, QObject *parent)
	: QObject(parent), station_info_manager_(db)
{
	sync_thread.SetStream(&audio_stream_);
	connect(&sync_thread,
	        &GuiSyncThread::SyncEvent,
	        this,
	        &HybridRadio::TunerSyncEvent);
	sync_thread.start();

	tuner_.SetCallbackNRSC5(
		[](const nrsc5_event_t *evt, void *opaque)
		{
			const auto radio = static_cast<HybridRadio *>(opaque);
			radio->NRSC5Callback(evt);
		},
		this);
}

HybridRadio::~HybridRadio()
{
	tuner_.Stop();
	if (audio_stream_.IsActive())
		audio_stream_.Stop();
	tuner_.Close();
	sync_thread.Stop();
}

UTILS::StatusCodes HybridRadio::StartDefault()
{
	UTILS::StatusCodes ret = SelectDefaultAudioDevice();
	if (ret != UTILS::StatusCodes::Ok)
	{
		emit TunerStatus(TunerAction::Open, ret);
		return ret;
	}

	ret = SetChannel(current_channel_);
	if (ret != UTILS::StatusCodes::Ok)
		return ret;

	ret = Start();
	if (ret != UTILS::StatusCodes::Ok)
		return ret;

	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes HybridRadio::SelectDefaultAudioDevice()
{
	// TODO: Implement a way to select the default audio device.
	//  That way, this function isn't needed. This is a temporary solution.
	if (!IsSdrActive())
		return UTILS::StatusCodes::NoDevice;

	if (audio_stream_.IsOpen())
		return UTILS::StatusCodes::Ok;

	std::shared_ptr<PortAudio::Device> audioDefaultDevice;

	/* select audio device if not chosen */
	getApp()->GetAudioSystem().DefaultOutputDevice(audioDefaultDevice);

	UTILS::StatusCodes ret = SetAudioDevice(audioDefaultDevice);
	if (ret != UTILS::StatusCodes::Ok)
	{
		emit TunerStatus(TunerAction::Open, ret);
		return ret;
	}

	Logger::Log(info,
	            "Selected Audio Device Automatically: {}",
	            audioDefaultDevice->Name());
	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes HybridRadio::Start()
{
	if (!IsSdrActive())
		return UTILS::StatusCodes::UnknownError;

	emit TunerStatus(TunerAction::Starting, UTILS::StatusCodes::Ok);

	if (!tuner_.Start())
	{
		emit TunerStatus(TunerAction::Start, UTILS::StatusCodes::TunerError);
		return UTILS::StatusCodes::TunerError;
	}

	// Start audio output
	if (audio_stream_.IsOpen())
	{
		int ret = audio_stream_.Start();
		if (ret < 0)
			Stop();
	}

	emit TunerStatus(TunerAction::Start, UTILS::StatusCodes::Ok);
	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes HybridRadio::Stop()
{
	if (!IsSdrActive())
		return UTILS::StatusCodes::UnknownError;

	emit TunerStatus(TunerAction::Stopping, UTILS::StatusCodes::Ok);

	bool success = tuner_.Stop();
	if (!success)
	{
		emit TunerStatus(TunerAction::Stop, UTILS::StatusCodes::TunerError);
		return UTILS::StatusCodes::TunerError;
	}

	// Stop audio output
	if (audio_stream_.IsActive())
	{
		int ret = audio_stream_.Stop();
		if (ret < 0)
			Logger::Log(err, "Failed to stop audio output");
	}

	emit TunerStatus(TunerAction::Stop, UTILS::StatusCodes::Ok);
	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes HybridRadio::ClearSDRDevice()
{
	if (!IsSdrActive())
		return UTILS::StatusCodes::UnknownError;

	if (const UTILS::StatusCodes ret = Stop(); ret != UTILS::StatusCodes::Ok)
		return ret;

	tuner_.Close();
	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes HybridRadio::SetSDRDevice(
	const PortSDR::Device &device)
{
	emit TunerStatus(TunerAction::Opening, UTILS::StatusCodes::Ok);

	UTILS::StatusCodes ret = tuner_.Open(device) ? UTILS::StatusCodes::Ok : UTILS::StatusCodes::TunerError;
	if (ret != UTILS::StatusCodes::Ok)
	{
		Logger::Log(err, "Failed to open SDR device");
		emit TunerStatus(TunerAction::Open, ret);
		return ret;
	}

	emit TunerStatus(TunerAction::Open, UTILS::StatusCodes::Ok);
	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes HybridRadio::SetAudioDevice(
	const std::shared_ptr<PortAudio::Device> &device)
{
	if (!device)
		return UTILS::StatusCodes::UnknownError;

	if (audio_stream_.IsOpen())
		audio_stream_.Close();

	if (device->isNull())
		return UTILS::StatusCodes::Ok;

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

	if (const int ret = audio_stream_.Open(params); ret < 0)
	{
		Logger::Log(err, "Failed to open audio output: {}", Pa_GetErrorText(ret));
		return UTILS::StatusCodes::UnknownError;
	}

	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes HybridRadio::SetChannel(
	const Modulation::Type type,
	const double frequency,
	unsigned int programId)
{
	if (!IsSdrActive())
		return UTILS::StatusCodes::UnknownError;

	return SetChannel({TunerOpts{type, frequency}, programId});
}

UTILS::StatusCodes HybridRadio::SetChannel(
	const Channel &channel)
{
	if (!IsSdrActive())
		return UTILS::StatusCodes::UnknownError;

	UTILS::StatusCodes ret = tuner_.SetTunerOptions(channel.tuner_opts);

	if (ret == UTILS::StatusCodes::Ok)
	{
		audio_stream_.Reset();
		station_info_ = channel.station_info;
	}
	else if (ret != UTILS::StatusCodes::Empty)
	{
		emit TunerStatus(TunerAction::ChangeChannel, ret);
		return ret;
	}

	{
		std::lock_guard lock(station_mutex_);

		station_info_.Reset();
		station_details_.Reset();
		ber_ = {};
	}

	SetProgram(channel.station_info.current_program);
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

	RadioStationUpdate(CreateChannel());
}

ActiveChannel HybridRadio::GetChannel() const
{
	if (!IsSdrActive())
		return {};

	std::lock_guard lock(station_mutex_);
	return CreateChannel();
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

void HybridRadio::NRSC5Callback(const nrsc5_event_t *evt)
{
	switch (evt->event)
	{
		case NRSC5_EVENT_SYNC:
		{
			Logger::Log(info, "HDRadio: Synchronized");
			Logger::Log(info, "HDRadio: Primary Service Mode: {}", evt->sync.psmi);
			HDSyncUpdate(true);
			m_sync_ = std::chrono::steady_clock::now();
			break;
		}
		case NRSC5_EVENT_LOST_SYNC:
		{
			Logger::Log(info, "HDRadio: Lost Sync");
			HDSyncUpdate(false);
			m_sync_.reset();
			break;
		}
		case NRSC5_EVENT_MER:
		{
			Logger::Log(info, "HDRadio: MER: {:.1f} dB (lower), {:.1f} dB (upper)", evt->mer.lower, evt->mer.upper);
			HDSignalStrengthUpdate(evt->mer.lower, evt->mer.upper);
			break;
		}
		case NRSC5_EVENT_BER:
		{
			const float cber = evt->ber.cber;

			ber_.Add(cber);

			Logger::Log(info,
			            "HDRadio: BER: {:.6f}, avg: {:.6f}, min: {:.6f}, max: {:.6f}",
			            cber,
			            ber_.ber,
			            ber_.min,
			            ber_.max);
			break;
		}
		/* sis as a hint for programs */
		case NRSC5_EVENT_AUDIO_SERVICE_DESCRIPTOR:
		{
			const unsigned int kProgramId = evt->asd.program;

			NRSC5::Program &program = station_details_.programs[kProgramId];
			program.type = evt->asd.type;

			Logger::Log(info,
			            "HD{}: Audio Service type={}",
			            NRSC5::FriendlyProgramId(kProgramId),
			            NRSC5::Decoder::ProgramTypeName(program.type));
			RadioStationUpdate(CreateChannel());
			break;
		}
		/* all existing programs get called here */
		case NRSC5_EVENT_AUDIO_SERVICE:
		{
			const unsigned int kProgramId = evt->audio_service.program;

			NRSC5::Program &program = station_details_.programs[kProgramId];
			program.type = evt->audio_service.type;

			Logger::Log(info,
			            "HD{}: Audio Service type={}",
			            NRSC5::FriendlyProgramId(kProgramId),
			            NRSC5::Decoder::ProgramTypeName(evt->audio_service.type));

			RadioStationUpdate(CreateChannel());
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
			station_info_.country_code = evt->station_id.country_code;
			station_info_.id = evt->station_id.fcc_facility_id;

			Logger::Log(info,
			            "HDRadio: Station ID: {} ({})",
			            station_info_.id,
			            station_info_.country_code);

			RadioStationUpdate(CreateChannel());
			break;
		}
		case NRSC5_EVENT_STATION_NAME:
		{
			station_info_.name = evt->station_name.name;

			Logger::Log(info, "HDRadio: Station Name: {}", station_info_.name);

			RadioStationUpdate(CreateChannel());
			break;
		}
		case NRSC5_EVENT_STATION_MESSAGE:
		{
			station_details_.message = evt->station_message.message;

			Logger::Log(info, "HDRadio: Station Message: {}", station_details_.message);
			break;
		}
		case NRSC5_EVENT_STATION_SLOGAN:
		{
			station_details_.slogan = evt->station_slogan.slogan;

			Logger::Log(info, "HDRadio: Station Slogan: {}", station_details_.slogan);
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
						NRSC5::Program &program = station_details_.programs[sig_component->audio.port];

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

			Logger::Log(debug,
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

			Logger::Log(debug,
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
		case NRSC5_EVENT_LOT_HEADER:
		{
			NRSC5::Lot lot(evt);

			Logger::Log(debug,
			            "HD{}: LOT Header: port={} id={} name={} size={} mime={} service={} expire={:%Y-%m-%dT%H:%M:%SZ} (in {})",
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
			break;
		}
		case NRSC5_EVENT_LOT:
		{
			NRSC5::Lot lot(evt);

			Logger::Log(info,
			            "HD{}: LOT File: port={} id={} name={} size={} mime={} service={} expire={:%Y-%m-%dT%H:%M:%SZ} (in {})",
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

			HDReceivedLot(station_info_, lot);
			break;
		}
		case NRSC5_EVENT_HDC:
		{
			if (evt->hdc.program == station_info_.current_program)
			{
				audio_packets++;
				audio_bytes += evt->hdc.count * sizeof(evt->hdc.data[0]);

				if (audio_packets >= 32)
				{
					Logger::Log(info,
					            "Audio bit rate: {:.1f} kbps",
					            static_cast<float>(audio_bytes)
					            * 8 * NRSC5_SAMPLE_RATE_AUDIO / NRSC5_AUDIO_FRAME_SAMPLES /
					            static_cast<float>(audio_packets) / 1000);
					audio_packets = 0;
					audio_bytes = 0;
				}
			}
			break;
		}
		case NRSC5_EVENT_ID3:
		{
			if (evt->hdc.program == station_info_.current_program)
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

				HDID3Update(kId3);
			}
			break;
		}
		case NRSC5_EVENT_AUDIO:
		{
			if (evt->audio.program != station_info_.current_program)
				break;

			NRSC5Audio(evt->audio.data, evt->audio.count);
			break;
		}
		default:
			break;
	}
}

/**
 * @brief Automatically ran when the radio channel is updated
 * This is not ran from the gui thread
 * @param channel
 */
void HybridRadio::RadioStationUpdate(const ActiveChannel &channel)
{
	sync_thread.Put(std::make_unique<GuiStationUpdate>(audio_stream_.GetWrittenTime(), channel));
}

void HybridRadio::HDSyncUpdate(bool sync)
{
	sync_thread.Put(std::make_unique<GuiHDSyncEvent>(audio_stream_.GetWrittenTime(), sync));
}

void HybridRadio::HDID3Update(const NRSC5::ID3 &id3)
{
	sync_thread.Put(std::make_unique<GuiID3Update>(audio_stream_.GetWrittenTime(), id3));
}

void HybridRadio::HDSignalStrengthUpdate(const float lower, const float upper)
{
	emit HDSignalStrength(lower, upper);
}

void HybridRadio::HDReceivedLot(const NRSC5::StationInfo &station,
                                const NRSC5::Lot &lot)
{
	// Update GUI cache with the LOT
	station_info_manager_.ReceiveLot(station, lot);
}

void HybridRadio::TunerSyncEvent(const std::shared_ptr<GuiSyncEvent> &event)
{
	switch (event->GetEventType())
	{
		case GuiSyncEvent::EventType::EVENT_HD_SYNC:
		{
			const auto syncEvent = std::dynamic_pointer_cast<GuiHDSyncEvent>(event);
			emit station_info_manager_.UpdateHDSync(syncEvent->on_);
			break;
		}
		case GuiSyncEvent::EventType::EVENT_HD_STATION:
		{
			const auto stationEvent = std::dynamic_pointer_cast<GuiStationUpdate>(event);
			station_info_manager_.StyleAndDisplayStation(stationEvent->channel_);
			break;
		}
		case GuiSyncEvent::EventType::EVENT_HD_ID3:
		{
			const auto id3Event = std::dynamic_pointer_cast<GuiID3Update>(event);
			station_info_manager_.StyleAndDisplayID3(id3Event->id3_);
			break;
		}
		default:
		{
			Logger::Log(warn, "Unhandled Sync Event: {}", static_cast<int>(event->GetEventType()));
			break;
		}
	}
}

ActiveChannel HybridRadio::CreateChannel() const
{
	assert(IsSdrActive());

	return {
		tuner_.GetTunerOptions(),
		station_info_,
		station_details_
	};
}
