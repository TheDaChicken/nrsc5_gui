//
// Created by TheDaChicken on 12/12/2025.
//

#include "HybridSession.h"

#include "utils/Log.h"

HybridSession::HybridSession()
{
}

void HybridSession::PushInput(std::shared_ptr<IRadioInput> &&input)
{
	input_ = std::move(input);
	input_->onCapabilityChanged = [this](const NRSC5::StreamSupported supported)
	{
		if (!decoder.Open(supported))
		{
			Logger::Log(err, "failed to open decoder");
		}
	};

	// Only now attach listener
	input_->onRadio = [this](const RadioFrame &transfer)
	{
		decoder.ProcessSamples(transfer);
	};

	auto supported = NRSC5::Processor::SelectStream(
		input_->GetCapabilities());
	if (!supported)
		return;

	if (!input_->SetCapabilities(*supported))
		return;

	decoder.onEvent = [](const nrsc5_event_t *evt)
	{
	};
}

InputStatus HybridSession::Start() const
{
	return input_->Start();
}

InputStatus HybridSession::Stop() const
{
	return input_->Stop();
}

InputStatus HybridSession::SetFrequency(const Band::Type type, const uint32_t freq)
{
	if (const auto control = input_->GetISDRControl())
	{
		control->SetFrequency(type, freq);

		decoder.Reset(freq); // full reset
		decoder.SetMode(type);
	}

	return INPUT_OK;
}
