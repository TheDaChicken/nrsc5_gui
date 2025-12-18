//
// Created by TheDaChicken on 7/12/2025.
//

#ifndef NRSC5PROCESSOR_H
#define NRSC5PROCESSOR_H

#include <memory>
#include <PortSDR.h>

#include "Decoder.h"
#include "dsp/ArbResampler.h"
#include "utils/Error.h"
#include "utils/Types.h"

// Decide how to handle the sdr stream.
// NRSC5 needs special handling per sdr device because of the sample rate
enum class TunerMode
{
	Empty = -1, // No tuner mode set
	Native = 0, // Use the native sample rate of the device
	ArbResampler = 1, // Use a resampler to convert the sample rate
};

namespace NRSC5
{
class SDRProcessor
{
	public:
		SDRProcessor();

		int Open(const PortSDR::Device &device,
		         const std::unique_ptr<PortSDR::Stream> &stream);
		void Process(const void *data, size_t size);

		void SetCallback(const nrsc5_callback_t callback, void *opaque)
		{
			nrsc5_decoder_.SetCallback(callback, opaque);
		}

		int Reset(float freq);

	private:
		[[nodiscard]] TunerMode DecideTunerMode(const PortSDR::Device &device) const;
		int SetupTunerNative(const std::unique_ptr<PortSDR::Stream> &stream);
		int SetupTunerResamplerQ15(const std::unique_ptr<PortSDR::Stream> &stream);
		int SetupTunerResamplerCCC(const std::unique_ptr<PortSDR::Stream> &stream);

		std::unique_ptr<ArbResamplerCCC> resampler_ccc;
		std::unique_ptr<ArbResamplerQ15> resampler_q15;
		std::vector<cint16_t> resampled_buffer_;
		double resampler_rate_ = 0;

		Decoder nrsc5_decoder_;

		TunerMode tuner_mode_ = TunerMode::Empty;
};
} // namespace NRSC5

#endif //NRSC5PROCESSOR_H
