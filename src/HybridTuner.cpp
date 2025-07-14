//
// Created by TheDaChicken on 7/1/2025.
//

#include "HybridTuner.h"

#include "utils/Log.h"

HybridTuner::HybridTuner()
{
}

HybridTuner::~HybridTuner()
{
}

bool HybridTuner::Open(const std::shared_ptr<PortSDR::Device> &device)
{
	if (!device)
	{
		Logger::Log(err, "Device is null");
		return false;
	}

	sdr_stream_.reset();

	int ret = device->CreateStream(sdr_stream_);
	if (ret < 0)
	{
		return false;
	}

	sdr_stream_->SetCallback([this](PortSDR::SDRTransfer &transfer)
	{
		SDRCallback(transfer);
	});

	ret = nrsc5_decoder_.Open(device, sdr_stream_);
	if (ret < 0)
	{
		sdr_stream_.reset();

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

	std::lock_guard lock(mutex_);

	if (int ret = nrsc5_decoder_.Reset(tunerOpts.freq) < 0)
	{
		Logger::Log(err, "Failed to reset NRSC5 decoder: {}", ret);
		return UTILS::StatusCodes::TunerError;
	}

	tuner_opts_ = tunerOpts;

	Logger::Log(debug, "HybridRadio: frequency has been set: {} hz", tunerOpts.freq);
	return UTILS::StatusCodes::Ok;
}

void HybridTuner::SDRCallback(
	PortSDR::SDRTransfer &sdr_transfer)
{
	if (sdr_transfer.dropped_samples > 0)
	{
		Logger::Log(warn, "SDR Dropped samples: {}", sdr_transfer.dropped_samples);
	}

	std::lock_guard lock(mutex_);

	nrsc5_decoder_.Process(sdr_transfer.data, sdr_transfer.frame_size);
}
