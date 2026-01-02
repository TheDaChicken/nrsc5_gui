//
// Created by TheDaChicken on 7/1/2025.
//

#include "HybridTuner.h"

#include "utils/Log.h"

HybridTuner::HybridTuner()
{
	decoder_.OpenPipe();
}

HybridTuner::~HybridTuner()
{
}

bool HybridTuner::Open(const PortSDR::Device &device)
{
	sdr_stream_.reset();

	auto errc = device.CreateStream(sdr_stream_);
	if (errc != PortSDR::ErrorCode::OK)
	{
		return false;
	}

	sdr_stream_->SetCallback([this](PortSDR::SDRTransfer &transfer)
	{
		SDRCallback(transfer);
	});

	NRSC5::StreamCapabilities capabilities;
	capabilities.native = device.host.lock()->GetType() == PortSDR::Host::RTL_SDR;
	capabilities.sample_rates = sdr_stream_->GetSampleRates();
	capabilities.sample_formats = sdr_stream_->GetSampleFormats();

	NRSC5::StreamSupported supported = NRSC5::Processor::SelectStream(capabilities);

	tuner_mode_ = supported.tuner_mode;

	auto perret = sdr_stream_->SetSampleFormat(supported.sample_format);
	if (perret != PortSDR::ErrorCode::OK)
	{
		Logger::Log(err,
		            "Failed to set sample format {}: {}",
		            static_cast<int>(supported.sample_format),
		            static_cast<int>(perret));
		return false;
	}

	perret = sdr_stream_->SetSampleRate(supported.sample_rate);
	if (perret != PortSDR::ErrorCode::OK)
	{
		Logger::Log(err,
		            "Failed to set sample rate {}: {}",
		            supported.sample_rate,
		            static_cast<int>(perret));
		return false;
	}

	if (tuner_mode_ == TunerMode::ArbResampler)
	{
		int ret = processor_.Open(supported);
		if (ret < 0)
		{
			sdr_stream_.reset();

			Logger::Log(err, "Failed to setup tuner: {}", ret);
			return false;
		}
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
	if (!sdr_stream_)
		return true;

	auto ret = sdr_stream_->Start();
	if (ret != PortSDR::ErrorCode::OK)
	{
		Logger::Log(err, "Failed to start SDR stream: {}", static_cast<int>(ret));
		return false;
	}

	return true;
}

bool HybridTuner::Stop()
{
	if (!sdr_stream_)
		return true;

	auto ret = sdr_stream_->Stop();
	if (ret != PortSDR::ErrorCode::OK)
	{
		Logger::Log(err, "Failed to stop SDR stream: {}", static_cast<int>(ret));
		return false;
	}
	return true;
}

UTILS::StatusCodes HybridTuner::SetTunerOptions(const TunerOpts &tunerOpts)
{
	if (!sdr_stream_)
		return UTILS::StatusCodes::UnknownError;

	if (sdr_stream_->GetCenterFrequency() == tunerOpts.freq)
		return UTILS::StatusCodes::Empty;

	if (auto ret = sdr_stream_->SetCenterFrequency(tunerOpts.freq);
		ret != PortSDR::ErrorCode::OK)
	{
		Logger::Log(err, "Failed to set SDR frequency {}: {}", static_cast<int>(ret), tunerOpts.freq);
		return UTILS::StatusCodes::TunerError;
	}

	std::lock_guard lock(mutex_);

	if (tuner_mode_ != TunerMode::Native)
	{
		if (int ret = processor_.Reset() < 0)
		{
			Logger::Log(err, "Failed to reset NRSC5 processor: {}", ret);
			return UTILS::StatusCodes::TunerError;
		}
	}

	if (int ret = decoder_.SetFrequency(tunerOpts.freq) < 0)
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

	if (tuner_mode_ == TunerMode::Native)
	{
		decoder_.SendIQ(static_cast<const uint8_t *>(sdr_transfer.data), sdr_transfer.frame_size * 2.0);
	}
	else
	{
		auto out = processor_.Process(sdr_transfer.data, sdr_transfer.frame_size);
		decoder_.SendIQ(static_cast<const int16_t *>(out.data), out.size * 2.0);
	}
}
