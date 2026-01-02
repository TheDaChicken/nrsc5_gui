//
// Created by TheDaChicken on 7/12/2025.
//

#include "Processor.h"

#include <fstream>

extern "C" {
#include <nrsc5.h>
}

#include "dsp/Firdes.h"
#include "dsp/Window.h"
#include "utils/Log.h"

// TODO: make this custom maybe?
constexpr int FILTER_TAP_COUNT = 64;

inline int16_t FloatToQ15(const float x)
{
	return static_cast<int16_t>(std::clamp(x, -1.0f, 1.0f) * 32767.0f);
}

tl::expected<NRSC5::StreamSupported, NRSC5::StreamStatus> NRSC5::Processor::SelectStream(
	const StreamCapabilities &params)
{
	switch (params.native)
	{
		case PortSDR::Host::RTL_SDR:
			return NativeStream(params);
		default:
			return ResamplerStream(params);
	}
}

NRSC5::Processor::Processor()
{
	resampler_stream_ = std::make_unique<FilterStream<cint16_t> >();
}

tl::expected<void, NRSC5::StreamStatus> NRSC5::Processor::Open(const StreamSupported &stream_supported)
{
	const auto ret = CreateResamplerQ15(stream_supported);
	if (ret != StreamStatus_Ok)
		return tl::unexpected(StreamStatus_ResamplerFailed);

	in_floats = stream_supported.sample_format == PortSDR::SAMPLE_FORMAT_IQ_FLOAT32;
	return {};
}

tl::expected<void, NRSC5::StreamStatus> NRSC5::Processor::Reset() const
{
	if (resampler_stream_)
		resampler_stream_->Reset();

	return {};
}

NRSC5::Processor::OutBuffer NRSC5::Processor::Process(const void *data, const size_t frame_size)
{
	const void *in = data;

	if (in_floats)
	{
		if (transit_buffer.size() < frame_size)
			transit_buffer.resize(frame_size);

		const auto float_ptr = static_cast<const float *>(data);
		const auto out_ptr = reinterpret_cast<int16_t *>(transit_buffer.data());

		for (size_t i = 0; i < frame_size * 2.0; i++)
		{
			out_ptr[i] = static_cast<int16_t>(float_ptr[i] * 32768.0f);
		}

		in = transit_buffer.data();
	}

	const std::size_t max_resample_size = std::ceil(
		frame_size * resampler_rate_) + FILTER_TAP_COUNT;

	// Reserve space for output
	if (resampled_buffer_.size() < max_resample_size)
		resampled_buffer_.resize(max_resample_size);

	const unsigned int resampled_size = resampler_stream_->IProcess(
		resampled_buffer_.data(),
		in,
		frame_size);
	return {resampled_buffer_.data(), resampled_size};
}

void NRSC5::Processor::SetMode(const Band::Type mode) const
{
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
	supported.tuner_mode = TunerMode::ArbResampler;

	assert(!params.sample_rates.empty());

	// Find the first sample rate bigger than NRSC5_SAMPLE_RATE_CS16_FM
	const auto samp_iter = std::find_if(
		params.sample_rates.begin(),
		params.sample_rates.end(),
		[](const uint32_t rate)
		{
			return rate >= NRSC5_SAMPLE_RATE_CS16_FM;
		});

	if (samp_iter == params.sample_rates.end())
		return tl::unexpected(StreamStatus_UnsupportedSampleRate);

	Logger::Log(debug, "SDR Sample Rate chosen: {}", *samp_iter);

	supported.sample_rate = *samp_iter;

	if (std::find(
		params.sample_formats.begin(),
		params.sample_formats.end(),
		PortSDR::SAMPLE_FORMAT_IQ_INT16) != params.sample_formats.end())
	{
		supported.sample_format = PortSDR::SAMPLE_FORMAT_IQ_INT16;
	}

	if (std::find(
		params.sample_formats.begin(),
		params.sample_formats.end(),
		PortSDR::SAMPLE_FORMAT_IQ_FLOAT32) != params.sample_formats.end())
	{
		supported.sample_format = PortSDR::SAMPLE_FORMAT_IQ_FLOAT32;
	}

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
	resampler_stream_->SetFilter(std::make_unique<ArbResampler<cint16_t, cint16_t> >(
		resampler_rate_,
		taps,
		FILTER_TAP_COUNT));

	Logger::Log(debug, "ArbResampler rate: {}", resampler_rate_);

	resampled_buffer_ = std::vector<cint16_t>(1024);
	return StreamStatus_Ok;
}
