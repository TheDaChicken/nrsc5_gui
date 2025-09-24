//
// Created by TheDaChicken on 7/12/2025.
//

#include "Processor.h"

#include "dsp/Firdes.h"
#include "dsp/Window.h"

// TODO: make this custom maybe?
constexpr int FILTER_TAP_COUNT = 64;

inline int16_t FloatToQ15(const float x)
{
	return static_cast<int16_t>(std::clamp(x, -1.0f, 1.0f) * 32767.0f);
}

static std::unique_ptr<nrsc5_t, decltype(&nrsc5_close)> nrsc5_open_pipe()
{
	nrsc5_t *st;
	// This won't happen. The function always returns 0.
	if (const int ret = nrsc5_open_pipe(&st); ret < 0)
		throw std::runtime_error("Failed to open NRSC5 pipe");

	return {st, nrsc5_close};
}

NRSC5::Processor::Processor()
	: nrsc5_decoder_(nrsc5_open_pipe())
{
}

tl::expected<NRSC5::StreamSupported, NRSC5::StreamStatus> NRSC5::Processor::Open(
	const StreamCapabilities &params)
{
	auto supported = SelectStream(params);
	if (!supported)
		return tl::unexpected(supported.error());

	if (supported->tuner_mode == TunerMode::ArbResampler)
	{
		auto ret = CreateResamplerQ15(*supported);
		if (ret != StreamStatus_Ok)
			return tl::unexpected(StreamStatus_ResamplerFailed);
	}

	tuner_mode_ = supported->tuner_mode;
	return supported;
}

tl::expected<void, NRSC5::StreamStatus> NRSC5::Processor::Reset(const float freq) const
{
	if (int ret = nrsc5_set_frequency(nrsc5_decoder_.get(), freq);
		ret < 0)
	{
		Logger::Log(err, "Failed to reset NRSC5 Decoder {}", ret);
		return tl::unexpected(StreamStatus_Error);
	}

	if (resampler_stream_)
		resampler_stream_->Reset();

	return {};
}

void NRSC5::Processor::Process(const void *data, const size_t frame_size)
{
	switch (tuner_mode_)
	{
		case TunerMode::Native:
		{
			nrsc5_pipe_samples_cu8(
				nrsc5_decoder_.get(),
				static_cast<const uint8_t *>(data),
				frame_size * 2);
			break;
		}
		case TunerMode::ArbResampler:
		{
			break;
		}
		default:
		{
			Logger::Log(err, "Unsupported tuner mode: {}", static_cast<int>(tuner_mode_));
			break;
		}
	}
}

void NRSC5::Processor::Resample(const void *data, const size_t frame_size)
{
	const std::size_t resampled_buffer_size = std::round(frame_size * resampler_rate_ + 1);

	// Reserve space for output
	if (resampled_buffer_.size() < resampled_buffer_size)
	{
		resampled_buffer_.resize(resampled_buffer_size);
	}

	const unsigned int resampled_size = resampler_stream_->IProcess(
		resampled_buffer_.data(),
		data,
		frame_size);
	if (resampled_size > 0)
	{
		nrsc5_pipe_samples_cs16(
			nrsc5_decoder_.get(),
			reinterpret_cast<const int16_t *>(resampled_buffer_.data()),
			resampled_size * 2);
	}
}

int ConvertToNRSC5Mode(Band::Type mode)
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

void NRSC5::Processor::SetMode(const Band::Type mode) const
{
	// TODO: The sample rate changes based on mode
	nrsc5_set_mode(nrsc5_decoder_.get(), ConvertToNRSC5Mode(mode));
}

tl::expected<NRSC5::StreamSupported, NRSC5::StreamStatus> NRSC5::Processor::SelectStream(
	const StreamCapabilities &params)
{
	switch (params.type)
	{
		case PortSDR::Host::RTL_SDR:
			return NativeStream(params);
		default:
			return ResamplerStream(params);
	}
}

tl::expected<NRSC5::StreamSupported, NRSC5::StreamStatus> NRSC5::Processor::NativeStream(
	const StreamCapabilities &params)
{
	StreamSupported supported{};
	supported.tuner_mode = TunerMode::Native;
	supported.sample_format = PortSDR::SAMPLE_FORMAT_IQ_UINT8;
	supported.sample_rate = NRSC5_SAMPLE_RATE_CU8;
	return supported;
}

tl::expected<NRSC5::StreamSupported, NRSC5::StreamStatus> NRSC5::Processor::ResamplerStream(
	const StreamCapabilities &params)
{
	StreamSupported supported{};

	assert(!params.sample_rates.empty());

	// Find the first sample rate bigger than NRSC5_SAMPLE_RATE_CS16_FM
	const auto iterator = std::find_if(
		params.sample_rates.begin(),
		params.sample_rates.end(),
		[](const uint32_t rate)
		{
			return rate >= NRSC5_SAMPLE_RATE_CS16_FM;
		});

	if (iterator == params.sample_rates.end())
		return tl::unexpected(StreamStatus_UnsupportedSampleRate);

	supported.tuner_mode = TunerMode::ArbResampler;
	supported.sample_format = PortSDR::SAMPLE_FORMAT_IQ_INT16;
	supported.sample_rate = *iterator;

	return supported;
}

NRSC5::StreamStatus
NRSC5::Processor::CreateResamplerQ15(
	const StreamSupported &supported)
{
	const std::vector<float_t> float_taps = firdes::low_pass_2(
		FILTER_TAP_COUNT,
		FILTER_TAP_COUNT * supported.sample_rate,
		NRSC5_CUTOFF_FREQ,
		NRSC5_TRANSITION_WIDTH,
		NRSC5_STOP_ATT,
		Window::WIN_KAISER,
		0.0);

	std::vector<cint16_t> taps;
	for (const auto &tap : float_taps)
	{
		taps.emplace_back(FloatToQ15(tap),
		                  FloatToQ15(tap));
	}

	resampler_rate_ = NRSC5_SAMPLE_RATE_CS16_FM / static_cast<double>(supported.sample_rate);
	Logger::Log(debug, "ArbResampler rate: {}", resampler_rate_);

	resampler_stream_ = std::make_unique<FilterStream<cint16_t> >();
	resampler_stream_->SetFilter(std::make_unique<ArbResampler<cint16_t, cint16_t> >(
		resampler_rate_,
		taps,
		FILTER_TAP_COUNT));

	resampled_buffer_ = std::vector<cint16_t>(1024);
	return StreamStatus_Ok;
}
