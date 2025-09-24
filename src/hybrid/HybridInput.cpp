//
// Created by TheDaChicken on 8/27/2025.
//

#include "HybridInput.h"

#include "input/SDRInput.h"

HybridInput::HybridInput()
{
}

void HybridInput::OpenSDR(const std::shared_ptr<PortSDR::Device> &device)
{
	auto input = std::make_unique<SDRInput>();
	if (!input->Open(device))
	{
		Logger::Log(err,
		            "Failed to open SDR device");
		return;
	}

	auto controller = std::make_shared<HybridController>();

	const auto supported = controller->Open(input->GetCapabilities());
	if (!supported)
	{
		Logger::Log(err,
		            "Failed to open HybridController for device: {}",
		            static_cast<int>(supported.error()));
		return;
	}

	auto format_result = input->SetSampleFormat(supported->sample_format);
	if (!format_result)
	{
		Logger::Log(err,
		            "Failed to set sample format {}: {}",
		            static_cast<int>(supported->sample_format),
		            format_result.error());
		return;
	}

	auto rate_result = input->SetSampleRate(supported->sample_rate);
	if (!rate_result)
	{
		Logger::Log(err,
		            "Failed to set sample rate {}: {}",
		            supported->sample_rate,
		            rate_result.error());
		return;
	}

	// Only now attach listener
	input->SetListener([this, controller](PortSDR::SDRTransfer &transfer)
	{
		Logger::Log(trace,
		            "SDR Transfer: frame_size={} dropped_samples={}",
		            transfer.frame_size,
		            transfer.dropped_samples);
		controller->ProcessSamples(transfer);
	});

	controller->onEvent = [this](const std::unique_ptr<EventData> &events)
	{
		session_manager_.HandleEvents(events);
	};
	controller->onAudioFrame = [this](const std::unique_ptr<StationAudioFrame> &audio_frame)
	{
		session_manager_.HandleAudio(audio_frame);
	};
	controller->onLotReceived = [this](const NRSC5::Lot &lot)
	{
		if (on_lot)
			on_lot(lot);
	};

	input_ = std::move(input);
	controller_ = std::move(controller);
}

void HybridInput::OpenFile(const std::string &path)
{
	// TODO: Implement file input
}

void HybridInput::SetChannel(const Station &station)
{
	if (!input_)
		return;

	const bool freq_changed = input_->GetChannel() != station.freq;

	if (freq_changed)
	{
		input_->SetChannel(station.freq);
		controller_->Reset(station.freq); // full reset
		controller_->SetMode(station.mode);

		Logger::Log(info,
		            "HybridInput: Tuning to {} Hz",
		            station.freq);

		auto new_frame = std::make_unique<FrequencyChangeFrame>();

		new_frame->freq = station.freq;
		new_frame->mode = station.mode;

		session_manager_.HandleEvents(std::move(new_frame));
	}

	{
		auto new_frame = std::make_unique<ProgramIdChangeFrame>();

		new_frame->program_id = station.program_id;

		session_manager_.HandleEvents(std::move(new_frame));
	}
}

void HybridInput::Play()
{
	if (!input_ || status_ == Playing)
		return;

	SetChannel(
		{
			static_cast<uint32_t>(93.3 * 1e6),
			Band::FM, 1
		});

	status_ = Playing;

	if (!input_->Start())
	{
		status_ = Error;
		return;
	}
}

void HybridInput::Stop()
{
	if (!input_ || status_ != Playing)
		return;

	status_ = Idle;

	input_->Stop();
	controller_->Reset(-1);
}

void HybridInput::NextChannel(const std::shared_ptr<HybridSession> &session) const
{
	const auto itr = session_manager_.GetPrograms().upper_bound(session->GetState().id);
	if (itr != session_manager_.GetPrograms().end())
	{
		session->SendProgram(itr->second);
	}
	else
	{
		// TODO: Scan for next Station
	}
}

void HybridInput::PreviousChannel(const std::shared_ptr<HybridSession> &session) const
{
	auto itr = session_manager_.GetPrograms().lower_bound(session->GetState().id);
	if (itr != session_manager_.GetPrograms().begin())
	{
		--itr;
		session->SendProgram(itr->second);
	}
	else
	{
		// TODO: Scan for previous Station
	}
}
