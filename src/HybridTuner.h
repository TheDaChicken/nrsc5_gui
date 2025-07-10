//
// Created by TheDaChicken on 7/1/2025.
//

#ifndef HYBRIDTUNER_H
#define HYBRIDTUNER_H

#include <fstream>
#include <QObject>
#include <memory>
#include <PortSDR.h>

#include "RadioChannel.h"
#include "dsp/ArbResampler.h"
#include "nrsc5/Decoder.h"
#include "utils/Error.h"
#include "utils/MessageQueue.h"

// Decide how to handle the sdr stream.
// NRSC5 needs special handling per sdr device because of the sample rate
enum class TunerMode
{
	Empty = -1, // No tuner mode set
	Native = 0, // Use the native sample rate of the device
	Resampler = 1, // Use a resampler to convert the sample rate
};

class HybridTuner final : public QObject
{
		Q_OBJECT

	public:
		HybridTuner();
		~HybridTuner();

		bool Open(const std::shared_ptr<PortSDR::Device> &device);
		void Close();

		bool Start();
		bool Stop();

		void SetCallbackNRSC5(nrsc5_callback_t callback, void *opaque)
		{
			nrsc5_decoder_.SetCallback(callback, opaque);
		}

		UTILS::StatusCodes SetTunerOptions(const TunerOpts &tunerOpts);

		bool IsSDRActive() const
		{
			return sdr_stream_ != nullptr;
		}

		const TunerOpts &GetTunerOptions() const
		{
			return tuner_opts_;
		}

		[[nodiscard]] PortSDR::Stream *GetSDRStream() const
		{
			return sdr_stream_.get();
		}

	signals:
		void TunerStream(PortSDR::Stream *stream);

	private:
		TunerMode DecideTunerMode(const std::shared_ptr<PortSDR::Device> &device) const;
		void SDRCallback(PortSDR::SDRTransfer &sdr_transfer);
		void ProcessThread();
		int SetupTunerNative();
		int SetupTunerResampler();

		TunerMode tuner_mode_ = TunerMode::Empty;

		std::mutex mutex_;
		std::unique_ptr<ArbResamplerQ15> resampler_;
		std::vector<cint16_t> resampled_buffer_;
		double resampler_rate_;

		std::unique_ptr<PortSDR::Stream> sdr_stream_;
		TunerOpts tuner_opts_;

		NRSC5::Decoder nrsc5_decoder_;
};

#endif //HYBRIDTUNER_H
