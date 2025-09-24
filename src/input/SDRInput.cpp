//
// Created by TheDaChicken on 8/27/2025.
//

#include "SDRInput.h"

bool SDRInput::Open(const std::shared_ptr<PortSDR::Device> &device)
{
	sdr_stream_.reset();

	int ret = device->CreateStream(sdr_stream_);
	if (ret < 0)
	{
		return false;
	}

	device_ = device;
	return true;
}

bool SDRInput::Start()
{
	assert(sdr_stream_);
	return sdr_stream_->Start() == 0;
}

bool SDRInput::Stop()
{
	assert(sdr_stream_);
	return sdr_stream_->Stop() == 0;
}

uint32_t SDRInput::GetChannel()
{
	assert(sdr_stream_);
	return sdr_stream_->GetCenterFrequency();
}

bool SDRInput::SetChannel(uint32_t freq)
{
	return sdr_stream_->SetCenterFrequency(freq, 0) == 0;
}

void SDRInput::SetListener(const PortSDR::Stream::SDR_CALLBACK &listener)
{
	assert(sdr_stream_);
	sdr_stream_->SetCallback(listener);
}

tl::expected<void, int> SDRInput::SetSampleFormat(PortSDR::SampleFormat sample_format)
{
	int ret = sdr_stream_->SetSampleFormat(sample_format);
	if (ret < 0)
		return tl::unexpected(ret);

	return {};
}

tl::expected<void, int> SDRInput::SetSampleRate(uint32_t sample_rate)
{
	int ret = sdr_stream_->SetSampleRate(sample_rate);
	if (ret < 0)
		return tl::unexpected(ret);

	return {};
}

NRSC5::StreamCapabilities SDRInput::GetCapabilities()
{
	NRSC5::StreamCapabilities capabilities;
	capabilities.type = device_->host->GetType();
	capabilities.sample_rates = sdr_stream_->GetSampleRates();
	capabilities.sample_formats = sdr_stream_->GetSampleFormats();
	return capabilities;
}

std::vector<std::string> SDRInput::GetGainModes()
{
	return sdr_stream_->GetGainModes();
}

tl::expected<void, int> SDRInput::SetGainMode(const std::string &mode)
{
	return sdr_stream_->SetGainModes(mode) == 0 ? tl::expected<void, int>{} : tl::unexpected(-1);
}

tl::expected<void, int> SDRInput::SetGain(std::string_view name, float gain)
{
	return sdr_stream_->SetGain(gain, name) == 0 ? tl::expected<void, int>{} : tl::unexpected(-1);
}

std::string SDRInput::GetGainMode()
{
	return sdr_stream_->GetGainMode();
}

std::vector<PortSDR::Gain> SDRInput::GetGainStages()
{
	return sdr_stream_->GetGainStages();
}

PortSDR::Gain SDRInput::GetGainStage()
{
	return sdr_stream_->GetGainStage();
}

double SDRInput::GetGain(const std::string_view name)
{
	return sdr_stream_->GetGain(name);
}
