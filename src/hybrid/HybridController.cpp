//
// Created by TheDaChicken on 8/3/2025.
//

#include "HybridController.h"

#include "../utils/Log.h"
#include "gui/managers/EventsDispatcher.h"

#include <fmt/chrono.h>

HybridController::HybridController()
{
	stream_processor_.SetCallback(NRSC5Callback, this);
}

HybridController::~HybridController()
= default;

tl::expected<NRSC5::StreamSupported, NRSC5::StreamStatus> HybridController::Open(
	const NRSC5::StreamCapabilities &input)
{
	return stream_processor_.Open(input);
}

void HybridController::Reset(const double freq)
{
	std::lock_guard lock(mutex_);
	stream_processor_.Reset(freq);
}

void HybridController::SetMode(Band::Type mode)
{
	std::lock_guard lock(mutex_);
	stream_processor_.SetMode(mode);
}

void HybridController::ProcessSamples(
	PortSDR::SDRTransfer &sdr_transfer)
{
	if (sdr_transfer.dropped_samples > 0)
	{
		Logger::Log(warn, "SDR Dropped samples: {}", sdr_transfer.dropped_samples);
	}

	std::lock_guard lock(mutex_);
	stream_processor_.Process(sdr_transfer.data, sdr_transfer.frame_size);
}

void HybridController::NRSC5Callback(const nrsc5_event_t *evt, void *opaque)
{
	const auto dispatcher = static_cast<HybridController *>(opaque);

	switch (evt->event)
	{
		case NRSC5_EVENT_SYNC:
		{
			Logger::Log(info, "HDRadio: Sync");
			Logger::Log(info,
			            "HDRadio: Freq offset {}",
			            evt->sync.freq_offset);
			Logger::Log(info, "HDRadio: Primary Service Mode: {}", evt->sync.psmi);
			break;
		}
		case NRSC5_EVENT_LOST_SYNC:
		{
			Logger::Log(warn, "HDRadio: Lost Sync");
			break;
		}
		case NRSC5_EVENT_STATION_ID:
		{
			auto new_frame = std::make_unique<StationIdFrame>();

			new_frame->country_code = evt->station_id.country_code;
			new_frame->id = evt->station_id.fcc_facility_id;

			Logger::Log(info,
			            "HDRadio: Station ID: {} ({})",
			            evt->station_id.fcc_facility_id,
			            evt->station_id.country_code);

			dispatcher->onEvent(std::move(new_frame));
			break;
		}
		case NRSC5_EVENT_MER:
		{
			Logger::Log(info,
			            "HDRadio: MER: {:.1f} dB (lower), {:.1f} dB (upper)",
			            evt->mer.lower,
			            evt->mer.upper);
			break;
		}
		case NRSC5_EVENT_STATION_NAME:
		{
			auto new_frame = std::make_unique<StationNameFrame>();

			new_frame->name = evt->station_name.name;

			Logger::Log(info,
			            "HDRadio: Station Name: {}",
			            evt->station_name.name);

			dispatcher->onEvent(std::move(new_frame));
			break;
		}
		case NRSC5_EVENT_AUDIO_SERVICE:
		{
			const char *program_type_str;
			auto new_frame = std::make_unique<StationProgramFrame>();

			new_frame->type = evt->audio_service.type;
			new_frame->id = evt->audio_service.program;

			nrsc5_program_type_name(
				evt->audio_service.type,
				&program_type_str
			);

			Logger::Log(info,
			            "HD{}: Audio Service type={}",
			            NRSC5::FriendlyProgramId(new_frame->id),
			            program_type_str);

			dispatcher->onEvent(std::move(new_frame));
			break;
		}
		case NRSC5_EVENT_AUDIO:
		{
			auto new_frame = std::make_unique<StationAudioFrame>();

			new_frame->program_id = evt->audio.program;
			new_frame->data = evt->audio.data;
			new_frame->size = evt->audio.count;

			dispatcher->onAudioFrame(new_frame);
			break;
		}
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
						// /* data service is associated with a program */
						// NRSC5::Program &program = station_details_.programs[sig_component->audio.port];
						//
						// program.name = sig_service->name;

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
		case NRSC5_EVENT_ID3:
		{
			auto new_frame = std::make_unique<StationId3Frame>();
			NRSC5::ID3 id3(evt);
			const unsigned int friendlyId = NRSC5::FriendlyProgramId(id3.program_id);

			if (!id3.artist.empty() || !id3.title.empty())
				Logger::Log(info,
				            "HD{}: Artist={} Title={}",
				            friendlyId,
				            id3.artist,
				            id3.title);

			if (!id3.album.empty())
				Logger::Log(info,
				            "HD{}: Album={}",
				            friendlyId,
				            id3.album);

			if (!id3.genre.empty())
				Logger::Log(info,
				            "HD{}: Genre={}",
				            friendlyId,
				            id3.genre);

			if (!id3.xhdr.Empty())
				Logger::Log(info,
				            "HD{}: XHDR: {} Lot={} mime={}",
				            friendlyId,
				            id3.xhdr.ParamName(),
				            id3.xhdr.lot,
				            NRSC5::DescribeMime(id3.xhdr.mime));

			new_frame->id3 = std::move(id3);

			dispatcher->onEvent(std::move(new_frame));
			break;
		}
		case NRSC5_EVENT_LOT:
		{
			NRSC5::Lot lot(evt);

			Logger::Log(info,
			            "HD{}: LOT File: port={} name={} mime={} service={} expire={:%Y-%m-%dT%H:%M:%SZ} (in {})",
			            lot.component.programId.has_value()
				            ? std::to_string(NRSC5::FriendlyProgramId(lot.component.programId.value()))
				            : "Radio",
			            lot.id,
			            lot.name,
			            NRSC5::DescribeMime(lot.mime),
			            NRSC5::DescribeMime(lot.component.mime),
			            lot.discard_utc,
			            std::chrono::duration_cast<std::chrono::seconds>(
				            lot.expire_point - std::chrono::system_clock::now()));

			dispatcher->onLotReceived(lot);
			break;
		}
		default:
			break;
	}
}
