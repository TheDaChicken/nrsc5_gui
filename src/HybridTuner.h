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
#include "nrsc5/Processor.h"
#include "utils/Error.h"
#include "utils/MessageQueue.h"

class HybridTuner final : public QObject
{
		Q_OBJECT

	public:
		HybridTuner();
		~HybridTuner();

		bool Open(const PortSDR::Device &device);
		void Close();

		bool Start();
		bool Stop();

		void SetCallbackNRSC5(const nrsc5_callback_t callback, void *opaque)
		{
			decoder_.SetCallback(callback, opaque);
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

		std::mutex mutex_;
		NRSC5::Processor processor_;
		NRSC5::Decoder decoder_;
		TunerMode tuner_mode_;
		std::unique_ptr<PortSDR::Stream> sdr_stream_;
		TunerOpts tuner_opts_;
};

#endif //HYBRIDTUNER_H
