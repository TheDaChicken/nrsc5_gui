//
// Created by TheDaChicken on 8/3/2025.
//

#ifndef HYBRIDTUNER_H
#define HYBRIDTUNER_H

#include <mutex>

#include "nrsc5/Decoder.h"
#include "utils/Frame.h"

#include "RadioChannel.h"
#include "nrsc5/Processor.h"

class HybridDecoder final
{
	public:
		explicit HybridDecoder();
		~HybridDecoder();

		tl::expected<void, NRSC5::StreamStatus> Open(const NRSC5::StreamSupported &input);

		void Reset(float freq);
		void SetMode(Band::Type mode);
		void ProcessSamples(const RadioFrame &transfer);

		std::function<void(const nrsc5_event_t *evt)> onEvent;

	private:
		std::mutex mutex_;
		TunerMode tuner_mode_ = TunerMode::Empty;
		NRSC5::Decoder decoder_;
		NRSC5::Processor stream_processor_;
};

#endif //HYBRIDTUNER_H
