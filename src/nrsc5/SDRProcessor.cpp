//
// Created by TheDaChicken on 7/12/2025.
//

#include "SDRProcessor.h"

#include "dsp/Firdes.h"
#include "dsp/Window.h"

#define NRSC5_CUTOFF_FREQ (372094)
#define NRSC5_TRANSITION_WIDTH (530000)
#define NRSC5_STOP_ATT (90)

NRSC5::SDRProcessor::SDRProcessor()
{
	// This won't happen. The function always returns 0.
	if (int ret = nrsc5_decoder_.OpenPipe(); ret < 0)
		throw std::runtime_error("Failed to open NRSC5 pipe");
}

int NRSC5::SDRProcessor::Open(
	const PortSDR::Device &device,
	const std::unique_ptr<PortSDR::Stream> &stream)
{
	int ret;

	tuner_mode_ = DecideTunerMode(device);

	switch (tuner_mode_)
	{
		case TunerMode::Native:
		{
			ret = SetupTunerNative(stream);
			break;
		}
		case TunerMode::ArbResampler:
		{
			ret = SetupTunerResamplerQ15(stream);
			break;
		}
		default:
		{
			ret = -1;
			break;
		}
	}

	return ret;
}

int NRSC5::SDRProcessor::SetupTunerNative(const std::unique_ptr<PortSDR::Stream> &stream)
{
	auto ret = stream->SetSampleRate(NRSC5_SAMPLE_RATE_CU8);
	if (ret != PortSDR::ErrorCode::OK)
	{
		Logger::Log(err, "Failed to set sample rate: {}", static_cast<int>(ret));
		return -1;
	}

	ret = stream->SetSampleFormat(PortSDR::SAMPLE_FORMAT_IQ_UINT8);
	if (ret != PortSDR::ErrorCode::OK)
	{
		Logger::Log(err, "Failed to set sample format: {}", static_cast<int>(ret));
		return -1;
	}

	return 0;
}

int NRSC5::SDRProcessor::SetupTunerResamplerQ15(
	const std::unique_ptr<PortSDR::Stream> &stream)
{
	std::vector<uint32_t> sample_rates = stream->GetSampleRates();
	if (sample_rates.empty())
	{
		Logger::Log(err, "No sample rates available for the SDR stream");
		return -1;
	}

	// Find the first sample rate bigger than NRSC5_SAMPLE_RATE_CS16_FM
	const auto iterator = std::find_if(
		sample_rates.begin(),
		sample_rates.end(),
		[](const uint32_t rate)
		{
			return rate >= NRSC5_SAMPLE_RATE_CS16_FM;
		});

	if (iterator == sample_rates.end())
	{
		Logger::Log(err, "No suitable sample rate found");
		return -1;
	}
	const uint32_t sample_rate = *iterator;

	auto ret = stream->SetSampleRate(sample_rate);
	if (ret != PortSDR::ErrorCode::OK)
	{
		Logger::Log(err, "Failed to set sample rate: {}", static_cast<int>(ret));
		return -1;
	}

	ret = stream->SetSampleFormat(PortSDR::SAMPLE_FORMAT_IQ_INT16);
	if (ret != PortSDR::ErrorCode::OK)
	{
		Logger::Log(err, "Failed to set sample format: {}", static_cast<int>(ret));
		return -1;
	}

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

	resampler_rate_ = NRSC5_SAMPLE_RATE_CS16_FM / static_cast<double>(sample_rate);
	resampler_q15 = std::make_unique<ArbResamplerQ15>(
		resampler_rate_,
		taps,
		flt_size);

	Logger::Log(debug, "SDR sample rate set to: {} Hz", sample_rate);
	Logger::Log(debug, "ArbResampler rate: {}", resampler_rate_);
	return 0;
}

int NRSC5::SDRProcessor::SetupTunerResamplerCCC(const std::unique_ptr<PortSDR::Stream> &stream)
{
	std::vector<uint32_t> sample_rates = stream->GetSampleRates();
	if (sample_rates.empty())
	{
		Logger::Log(err, "No sample rates available for the SDR stream");
		return -1;
	}

	// Find the first sample rate bigger than NRSC5_SAMPLE_RATE_CS16_FM
	const auto iterator = std::find_if(
		sample_rates.begin(),
		sample_rates.end(),
		[](const uint32_t rate)
		{
			return rate >= NRSC5_SAMPLE_RATE_CS16_FM;
		});

	if (iterator == sample_rates.end())
	{
		Logger::Log(err, "No suitable sample rate found");
		return -1;
	}
	const uint32_t sample_rate = *iterator;

	auto ret = stream->SetSampleRate(sample_rate);
	if (ret != PortSDR::ErrorCode::OK)
	{
		Logger::Log(err, "Failed to set sample rate: {}", static_cast<int>(ret));
		return -1;
	}

	ret = stream->SetSampleFormat(PortSDR::SAMPLE_FORMAT_IQ_INT16);
	if (ret != PortSDR::ErrorCode::OK)
	{
		Logger::Log(err, "Failed to set sample format: {}", static_cast<int>(ret));
		return -1;
	}

	int flt_size = 64; // Number of filter taps
	const std::vector<float_t> float_taps = firdes::low_pass_2(
		flt_size,
		flt_size * sample_rate,
		NRSC5_CUTOFF_FREQ,
		NRSC5_TRANSITION_WIDTH,
		NRSC5_STOP_ATT,
		Window::WIN_KAISER,
		0.0);

	std::vector<cfloat_t> taps;
	for (const auto &tap : float_taps)
	{
		taps.emplace_back(static_cast<int16_t>(tap), static_cast<int16_t>(tap));
	}

	resampler_rate_ = NRSC5_SAMPLE_RATE_CS16_FM / static_cast<double>(sample_rate);
	resampler_ccc = std::make_unique<ArbResamplerCCC>(
		resampler_rate_,
		taps,
		flt_size);

	Logger::Log(debug, "SDR sample rate set to: {} Hz", sample_rate);
	Logger::Log(debug, "ArbResampler rate: {}", resampler_rate_);
	return 0;
}

void NRSC5::SDRProcessor::Process(const void *data, const size_t frame_size)
{
	switch (tuner_mode_)
	{
		case TunerMode::Native:
		{
			nrsc5_decoder_.SendIQ(static_cast<const uint8_t *>(data), frame_size * 2);
			break;
		}
		case TunerMode::ArbResampler:
		{
			const std::size_t resampled_buffer_size = frame_size * resampler_rate_ + 1;

			// Reserve space for output
			if (resampled_buffer_.size() < resampled_buffer_size)
			{
				resampled_buffer_.resize(resampled_buffer_size);
			}

			const int resampled_size = resampler_q15->Process(resampled_buffer_.data(),
			                                                  static_cast<const cint16_t *>(data),
			                                                  frame_size);
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

int NRSC5::SDRProcessor::Reset(float freq)
{
	if (int ret = nrsc5_decoder_.SetFrequency(freq);
		ret < 0)
	{
		Logger::Log(err, "Failed to reset NRSC5 Decoder {}", ret);
		return ret;
	}

	if (resampler_q15)
		resampler_q15->Reset();

	return 0;
}

TunerMode NRSC5::SDRProcessor::DecideTunerMode(const PortSDR::Device &device) const
{
	switch (device.host.lock()->GetType())
	{
		case PortSDR::Host::RTL_SDR:
		{
			// RTL-SDR uses native mode by default.
			// This allows me to use nrsc5 halfband filtering.
			return TunerMode::Native;
		}
		default:
		{
			return TunerMode::ArbResampler;
		}
	}
}
