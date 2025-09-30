//
// Created by TheDaChicken on 7/12/2025.
//

#ifndef NRSC5PROCESSOR_H
#define NRSC5PROCESSOR_H

#include <fstream>
#include <memory>
#include <PortSDR.h>

#include "utils/Band.h"

extern "C" {
#include <nrsc5.h>
}

#include "../dsp/resampler/ArbResampler.h"
#include "dsp/resampler/IFilterStream.h"
#include "utils/Expected.hpp"
#include "utils/Types.h"

// Decide how to handle the sdr stream.
// NRSC5 needs special handling per sdr device because of the sample rate
enum class TunerMode
{
	Empty = -1, // No tuner mode set
	Native = 0, // Use the native sample rate of the device
	ArbResampler = 1, // Use a resampler to convert the sample rate
};

#define NRSC5_CUTOFF_FREQ (372094)
#define NRSC5_TRANSITION_WIDTH (530000)
#define NRSC5_STOP_ATT (90)

namespace NRSC5
{
struct StreamCapabilities
{
	PortSDR::Host::HostType type;
	std::vector<uint32_t> sample_rates{};
	std::vector<PortSDR::SampleFormat> sample_formats{};
};

struct StreamSupported
{
	uint32_t sample_rate;
	PortSDR::SampleFormat sample_format;
	TunerMode tuner_mode;
};

enum StreamStatus
{
	StreamStatus_Ok = 0,
	StreamStatus_Error = -1,
	StreamStatus_UnsupportedSampleRate,
	StreamStatus_ResamplerFailed,
};

class Processor
{
	public:
		struct OutBuffer
		{
			const void *data;
			std::size_t size;
		};

		static tl::expected<StreamSupported, StreamStatus> SelectStream(const StreamCapabilities &params);

		Processor();

		tl::expected<void, StreamStatus> Open(const StreamSupported &stream_supported);
		tl::expected<void, StreamStatus> Reset() const;

		void SetMode(Band::Type mode) const;
		OutBuffer Process(const void *data, size_t size);

	private:
		static tl::expected<StreamSupported, StreamStatus> NativeStream(const StreamCapabilities &params);
		static tl::expected<StreamSupported, StreamStatus> ResamplerStream(const StreamCapabilities &params);

		StreamStatus CreateResamplerQ15(
			const StreamSupported &supported);

		std::unique_ptr<IFilterStream> resampler_stream_;
		HistoryBuffer<cint16_t> resampler_history_;
		std::vector<cint16_t> resampled_buffer_;
		std::vector<cint16_t> transit_buffer;

		double resampler_rate_ = 0;
		bool in_floats = false;
};
} // namespace NRSC5

#endif //NRSC5PROCESSOR_H
