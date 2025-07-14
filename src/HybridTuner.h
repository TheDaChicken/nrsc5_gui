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
#include "nrsc5/SDRProcessor.h"
#include "utils/Error.h"
#include "utils/MessageQueue.h"

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
		void SDRCallback(PortSDR::SDRTransfer &sdr_transfer);
		void ProcessThread();
		int SetupTunerNative(std::unique_ptr<PortSDR::Stream> &stream);
		int SetupTunerResampler(const std::unique_ptr<PortSDR::Stream> &stream);

		std::mutex mutex_;
		NRSC5::SDRProcessor nrsc5_decoder_;
		std::unique_ptr<PortSDR::Stream> sdr_stream_;
		TunerOpts tuner_opts_;
};

#endif //HYBRIDTUNER_H
