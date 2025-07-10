//
// Created by TheDaChicken on 7/1/2025.
//

#include "HybridTuner.h"

#include <nrsc5.h>

#include "dsp/Firdes.h"
#include "utils/Log.h"

#define NRSC5_CUTOFF_FREQ (372094)
#define NRSC5_TRANSITION_WIDTH (530000)
#define NRSC5_STOP_ATT (90)

HybridTuner::HybridTuner()
{
	// This won't happen. The function always returns 0.
	if (int ret = nrsc5_decoder_.OpenPipe(); ret < 0)
		throw std::runtime_error("Failed to open NRSC5 pipe");
}

HybridTuner::~HybridTuner()
{
	Stop();
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

	sdr_stream_->SetCallback([this](PortSDR::SDRTransfer& transfer)
	{
		SDRCallback(transfer);
	});

	switch (tuner_mode_)
	{
		case TunerMode::Native:
		{
			ret = SetupTunerNative();
			break;
		}
		case TunerMode::Resampler:
		{
			ret = SetupTunerResampler();
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

	if (resampler_)
	{
		std::lock_guard lock(mutex_);
		resampler_->Reset();
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

	ret = sdr_stream_->SetSampleFormat(PortSDR::SAMPLE_FORMAT_IQ_UINT8);
	if (ret < 0)
	{
		Logger::Log(err, "Failed to set sample format: " + std::to_string(ret));
		return ret;
	}

	return 0;
}

int HybridTuner::SetupTunerResampler()
{
	std::vector<uint32_t> sample_rates = sdr_stream_->GetSampleRates();
	if (sample_rates.empty())
	{
		Logger::Log(err, "No sample rates available for the SDR stream");
		return -1;
	}

	// Find the first sample rate bigger than NRSC5_SAMPLE_RATE_CS16_FM
	const auto it = std::find_if(sample_rates.begin(),
	                             sample_rates.end(),
	                             [](const uint32_t rate) { return rate >= NRSC5_SAMPLE_RATE_CS16_FM; });
	if (it == sample_rates.end())
	{
		return -1; // No suitable sample rate found
	}
	const uint32_t sample_rate = *it;

	int ret = sdr_stream_->SetSampleRate(sample_rate);
	if (ret < 0)
	{
		Logger::Log(err, "Failed to set sample rate: " + std::to_string(ret));
		return ret;
	}

	Logger::Log(debug, "SDR sample rate set to: {} Hz", sample_rate);

	resampler_rate_ = NRSC5_SAMPLE_RATE_CS16_FM / static_cast<double>(sample_rate);

	int flt_size = 64; // Number of filter taps
	const std::vector<float_t> float_taps = firdes::low_pass_2(
		flt_size,
		flt_size * sample_rate,
		NRSC5_CUTOFF_FREQ,
		NRSC5_TRANSITION_WIDTH,
		NRSC5_STOP_ATT,
		Window::WIN_KAISER,
		0.0);

	std::vector<cint16_t> taps;
	for (const auto &tap : float_taps)
	{
		taps.emplace_back(static_cast<int16_t>(tap * 32767.0f),
		                  static_cast<int16_t>(tap * 32767.0f));
	}

	resampler_ = std::make_unique<ArbResamplerQ15>(
		resampler_rate_,
		taps,
		flt_size);

	Logger::Log(debug, "Resampler rate: {}", resampler_rate_);

	ret = sdr_stream_->SetSampleFormat(PortSDR::SAMPLE_FORMAT_IQ_INT16);
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
			// RTL-SDR uses native mode by default.
			// This allows me to use nrsc5 halfband filtering.
			return TunerMode::Native;
		}
		default:
		{
			return TunerMode::Resampler;
		}
	}
}

void HybridTuner::SDRCallback(
	PortSDR::SDRTransfer &sdr_transfer)
{
	std::lock_guard lock(mutex_);

	switch (tuner_mode_)
	{
		case TunerMode::Native:
		{
			nrsc5_decoder_.SendIQ(static_cast<const uint8_t *>(sdr_transfer.data), sdr_transfer.frame_size * 2);
			break;
		}
		case TunerMode::Resampler:
		{
			// Reserve space for output
			if (resampled_buffer_.size() < sdr_transfer.frame_size * resampler_rate_ + 1)
			{
				resampled_buffer_.resize(sdr_transfer.frame_size * resampler_rate_ + 1);
			}

			const int resampled_size = resampler_->Process(resampled_buffer_.data(),
														   static_cast<const cint16_t *>(sdr_transfer.data),
														   sdr_transfer.frame_size);
			if (resampled_size > 0)
			{
				nrsc5_decoder_.SendIQ(reinterpret_cast<const int16_t *>(resampled_buffer_.data()),
									  resampled_size * 2);
			}
			break;
		}
		default:
		{
			Logger::Log(err, "Unsupported tuner mode: {}", static_cast<int>(tuner_mode_));
			break;
		}
	}
}
