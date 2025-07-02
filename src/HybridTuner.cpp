//
// Created by TheDaChicken on 7/1/2025.
//

#include "HybridTuner.h"

#include <nrsc5.h>

#include "utils/Log.h"

HybridTuner::HybridTuner()
{
	// This won't happen. The function always returns 0.
	if (int ret = nrsc5_decoder_.OpenPipe(); ret < 0)
		throw std::runtime_error("Failed to open NRSC5 pipe");
}

bool HybridTuner::Open(const std::shared_ptr<PortSDR::Device> &device)
{
	if (!device)
	{
		Logger::Log(err, "Device is null");
		return false;
	}

	tuner_mode_ = DecideTunerMode(device);
	if (tuner_mode_ == TunerMode::Empty)
	{
		Logger::Log(err, "Device {} is not supported", device->host->GetTypeName());
		return false;
	}

	int ret = device->CreateStream(sdr_stream_);
	if (ret < 0)
	{
		return false;
	}

	sdr_stream_->SetCallback([this](const void *data, const std::size_t elementSize)
	{
		SDRCallback(data, elementSize);
	});

	switch (tuner_mode_)
	{
		case TunerMode::Native:
		{
			ret = SetupTunerNative();
			break;
		}
		default:
		{
			ret = -1;
			break;
		}
	}

	if (ret < 0)
	{
		Logger::Log(err, "Failed to setup tuner: " + std::to_string(ret));
		return false;
	}

	emit TunerStream(sdr_stream_.get());
	return true;
}

void HybridTuner::Close()
{
	if (sdr_stream_)
		sdr_stream_.reset();

	emit TunerStream(nullptr);
}

bool HybridTuner::Start()
{
	int ret = sdr_stream_->Start();
	if (ret < 0)
	{
		Logger::Log(err, "Failed to start SDR stream: {}", ret);
		return false;
	}

	return true;
}

bool HybridTuner::Stop()
{
	int ret = sdr_stream_->Stop();
	if (ret < 0)
	{
		Logger::Log(err, "Failed to stop SDR stream: {}", ret);
		return false;
	}
	return true;
}

UTILS::StatusCodes HybridTuner::SetTunerOptions(const TunerOpts &tunerOpts)
{
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
		return UTILS::StatusCodes::TunerError;
	}

	tuner_opts_ = tunerOpts;

	Logger::Log(debug, "HybridRadio: frequency has been set: {} hz", tunerOpts.freq);
	return UTILS::StatusCodes::Ok;
}

int HybridTuner::SetupTunerNative()
{
	int ret = sdr_stream_->SetSampleRate(NRSC5_SAMPLE_RATE_CU8);
	if (ret < 0)
	{
		Logger::Log(err, "Failed to set sample rate: " + std::to_string(ret));
		return ret;
	}

	ret = sdr_stream_->SetSampleFormat(PortSDR::SAMPLE_FORMAT_UINT8);
	if (ret < 0)
	{
		Logger::Log(err, "Failed to set sample format: " + std::to_string(ret));
		return ret;
	}

	return 0;
}

TunerMode HybridTuner::DecideTunerMode(const std::shared_ptr<PortSDR::Device> &device) const
{
	switch (device->host->GetType())
	{
		case PortSDR::Host::RTL_SDR:
		{
			// RTL-SDR uses native mode by default
			return TunerMode::Native;
		}
		default:
		{
			return TunerMode::Empty;
		}
	}
}

void HybridTuner::SDRCallback(
	const void *data,
	const std::size_t frame_size)
{
	switch (tuner_mode_)
	{
		case TunerMode::Native:
		{
			nrsc5_decoder_.SendIQ(static_cast<const uint8_t *>(data), frame_size);
			break;
		}
		default:
		{
			Logger::Log(err, "Unsupported tuner mode: {}", static_cast<int>(tuner_mode_));
			break;
		}
	}
}
