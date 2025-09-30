//
// Created by TheDaChicken on 8/3/2025.
//

#include "HybridController.h"

#include "../utils/Log.h"
#include "gui/managers/EventsDispatcher.h"

#include <fmt/chrono.h>

static std::unique_ptr<nrsc5_t, decltype(&nrsc5_close)> nrsc5_open_pipe()
{
	nrsc5_t *st;
	// This won't happen. The function always returns 0.
	if (const int ret = nrsc5_open_pipe(&st); ret < 0)
		throw std::runtime_error("Failed to open NRSC5 pipe");

	return {st, nrsc5_close};
}

HybridController::HybridController()
	: nrsc5_decoder_(nrsc5_open_pipe())
{
	nrsc5_set_callback(nrsc5_decoder_.get(), NRSC5Callback, this);
}

HybridController::~HybridController()
= default;

tl::expected<void, NRSC5::StreamStatus> HybridController::Open(
	const NRSC5::StreamSupported &input)
{
	tuner_mode_ = input.tuner_mode;

	return stream_processor_.Open(input);
}

void HybridController::Reset(const double freq)
{
	std::lock_guard lock(mutex_);

	if (int ret = nrsc5_set_frequency(nrsc5_decoder_.get(), freq);
		ret < 0)
	{
		Logger::Log(err, "Failed to reset NRSC5 Decoder {}", ret);
		//return tl::unexpected(StreamStatus_Error);
	}

	stream_processor_.Reset();
}

int ConvertToNRSC5Mode(const Band::Type mode)
{
	switch (mode)
	{
		case Band::FM:
			return NRSC5_MODE_FM;
		case Band::AM:
			return NRSC5_MODE_AM;
		default:
			return NRSC5_MODE_FM;
	}
}

void HybridController::SetMode(Band::Type mode)
{
	std::lock_guard lock(mutex_);

	// TODO: The sample rate changes based on mode
	nrsc5_set_mode(nrsc5_decoder_.get(), ConvertToNRSC5Mode(mode));

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

	switch (tuner_mode_)
	{
		case TunerMode::Native:
		{
			nrsc5_pipe_samples_cu8(
				nrsc5_decoder_.get(),
				static_cast<const uint8_t *>(sdr_transfer.data),
				sdr_transfer.frame_size * 2);
			break;
		}
		case TunerMode::ArbResampler:
		{
			auto [data, size] = stream_processor_.Process(
				sdr_transfer.data,
				sdr_transfer.frame_size);

			nrsc5_pipe_samples_cs16(
				nrsc5_decoder_.get(),
				static_cast<const int16_t *>(data),
				size * 2);
			break;
		}
		default:
		{
			Logger::Log(err, "Unsupported tuner mode: {}", static_cast<int>(tuner_mode_));
			break;
		}
	}
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
