//
// Created by TheDaChicken on 8/27/2025.
//

#include "SDRInput.h"

#include <Error.h>

#include "utils/Log.h"

int ConvertSampleFormatToCount(const PortSDR::SampleFormat format)
{
	switch (format)
	{
		case PortSDR::SAMPLE_FORMAT_IQ_INT16:
			return sizeof(cint16_t);
		case PortSDR::SAMPLE_FORMAT_IQ_UINT8:
			return sizeof(cuint8_t);
		default:
			return 1;
	}
}

SDRInput::SDRInput(std::unique_ptr<PortSDR::Stream> stream)
	: sdr_stream_(std::move(stream))
{
	assert(sdr_stream_);

	sdr_stream_->SetCallback([this](PortSDR::SDRTransfer &transfer)
	{
		const std::size_t bytes = transfer.frame_size *
				ConvertSampleFormatToCount(transfer.format);

		Logger::Log(trace,
		            "SDR Transfer: frame_size={} dropped_samples={}",
		            transfer.frame_size,
		            transfer.dropped_samples);

		if (transfer.dropped_samples > 0)
		{
			Logger::Log(warn,
			            "SDR Dropped samples: {}",
			            transfer.dropped_samples);
		}

		// TODO: copy?????
		RadioFrame pkt;
		pkt.data.resize(bytes);
		pkt.frame_size = transfer.frame_size;
		memcpy(pkt.data.data(), transfer.data, bytes);

		if (onRadio)
			onRadio(pkt);
	});
}

bool SDRInput::IsStreaming()
{
	return running;
}

bool SDRInput::SetCapabilities(const NRSC5::StreamSupported &supported)
{
	auto format_result = SetSampleFormat(supported.sample_format);
	if (!format_result)
	{
		Logger::Log(err,
		            "Failed to set sample format {}: {}",
		            static_cast<int>(supported.sample_format),
		            format_result.error());
		return false;
	}

	auto rate_result = SetSampleRate(supported.sample_rate);
	if (!rate_result)
	{
		Logger::Log(err,
		            "Failed to set sample rate {}: {}",
		            supported.sample_rate,
		            rate_result.error());
		return false;
	}

	if (onCapabilityChanged)
		onCapabilityChanged(supported);
	return true;
}

InputStatus SDRInput::Start()
{
	assert(sdr_stream_);
	const PortSDR::ErrorCode ret = sdr_stream_->Start();
	if (ret == PortSDR::ErrorCode::OK)
	{
		running = true;
	}
	return ret == PortSDR::ErrorCode::OK ? INPUT_OK : INPUT_UNKNOWN;
}

InputStatus SDRInput::Stop()
{
	if (!sdr_stream_)
		return INPUT_UNKNOWN;
	const PortSDR::ErrorCode ret = sdr_stream_->Stop();
	if (ret == PortSDR::ErrorCode::OK)
	{
		running = false;
	}
	return ret == PortSDR::ErrorCode::OK ? INPUT_OK : INPUT_UNKNOWN;
}

uint32_t SDRInput::GetFrequency()
{
	return sdr_stream_->GetCenterFrequency();
}

bool SDRInput::SetFrequency(const Band::Type band, const uint32_t freq)
{
	sdr_stream_->SetCenterFrequency(freq);
	onFrequencyChanged(band, freq);
	return true;
}

tl::expected<void, int> SDRInput::SetSampleFormat(PortSDR::SampleFormat sample_format)
{
	PortSDR::ErrorCode ret = sdr_stream_->SetSampleFormat(sample_format);
	if (ret != PortSDR::ErrorCode::OK)
		return tl::unexpected(-1);

	return {};
}

tl::expected<void, int> SDRInput::SetSampleRate(uint32_t sample_rate)
{
	PortSDR::ErrorCode ret = sdr_stream_->SetSampleRate(sample_rate);
	if (ret != PortSDR::ErrorCode::OK)
		return tl::unexpected(-1);

	return {};
}

NRSC5::StreamCapabilities SDRInput::GetCapabilities() const
{
	NRSC5::StreamCapabilities capabilities;


	// Check if NRSC5_SAMPLE_RATE_CS8 is supported

	capabilities.native = true; // TODO: PLEASE CHECK THIS
	//capabilities.type = device_.host.lock()->GetType();
	capabilities.sample_rates = sdr_stream_->GetSampleRates();
	capabilities.sample_formats = sdr_stream_->GetSampleFormats();
	return capabilities;
}

void SDRInput::ResetBuffer()
{
}

std::vector<PortSDR::GainMode> SDRInput::GetGainModes()
{
	if (!sdr_stream_)
		return {};

	return sdr_stream_->GetGainModes();
}

int SDRInput::SetGainMode(PortSDR::GainMode mode)
{
	const auto ret = sdr_stream_->SetGainMode(mode);
	if (ret != PortSDR::ErrorCode::OK)
	{
		return -1;
	}

	return 0;
}

tl::expected<void, int> SDRInput::SetGain(const std::string_view name, float gain)
{
	return sdr_stream_->SetGain(gain, name) == PortSDR::ErrorCode::OK ? tl::expected<void, int>{} : tl::unexpected(-1);
}

PortSDR::GainMode SDRInput::GetGainMode()
{
	if (!sdr_stream_)
		return PortSDR::GAIN_MODE_FREE;

	return sdr_stream_->GetGainMode();
}

std::vector<PortSDR::Gain> SDRInput::GetGainStages()
{
	if (!sdr_stream_)
		return {};

	return sdr_stream_->GetGainStages();
}

double SDRInput::GetGain(const std::string_view name)
{
	return sdr_stream_->GetGain(name);
}

/*
SDRHost::SDRHost()
{
}

tl::expected<std::unique_ptr<IRadioInput>, int> SDRHost::Open(const PortSDR::Device &device)
{
	std::unique_ptr<PortSDR::Stream> stream;

	PortSDR::ErrorCode ret = device.CreateStream(stream);
	if (ret != PortSDR::ErrorCode::OK)
		return tl::unexpected(-1);

	return std::make_unique<SDRInput>(device, std::move(stream));
}

std::vector<SDRInfo> SDRHost::GetDevices()
{
	const std::vector<PortSDR::Device> devices = sdr_.GetDevices();

	std::vector<SDRInfo> new_devices;
	PortSDR::SDRInfo info;

	new_devices.reserve(devices.size());

	for (const auto &device : devices)
	{
		auto expect = Open(device);
		if (!expect)
			continue;

		const auto stream = std::move(*expect);

		new_devices.push_back(stream->GetDeviceInfo());
	}

	return new_devices;
}
*/

