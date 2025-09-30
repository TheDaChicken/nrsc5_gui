//
// Created by TheDaChicken on 8/3/2025.
//

#ifndef HYBRIDTUNER_H
#define HYBRIDTUNER_H

#include <memory>
#include <mutex>

extern "C" {
#include <nrsc5.h>
}

#include <PortSDR.h>

#include "RadioChannel.h"
#include "../gui/managers/EventsDispatcher.h"
#include "nrsc5/Processor.h"

struct FrequencyChangeFrame final : EventData
{
	FrequencyChangeFrame()
		: EventData(FREQUENCY_CHANGE)
	{
	}

	Band::Type mode{Band::Type::FM};
	uint32_t freq{0};
};

struct ProgramIdChangeFrame final : EventData
{
	explicit ProgramIdChangeFrame()
		: EventData(PROGRAM_CHANGE)
	{
	}

	unsigned int program_id = 0;
};

struct StationNameFrame final : EventData
{
	explicit StationNameFrame()
		: EventData(STATION_NAME)
	{
	}

	std::string name;
};

struct StationIdFrame final : EventData
{
	StationIdFrame() : EventData(STATION_ID)
	{
	}

	std::string country_code;
	unsigned int id{0};
};

struct StationProgramFrame final : EventData
{
	StationProgramFrame()
		: EventData(STATION_PROGRAM)
	{
	}

	unsigned int id{0};
	unsigned int type{NRSC5_PROGRAM_TYPE_UNDEFINED};
};

struct StationAudioFrame final : EventData
{
	StationAudioFrame() : EventData(AUDIO_FRAME), program_id(0), data(nullptr), size(0)
	{
	}

	unsigned int program_id;
	const int16_t *data;
	size_t size;
};

struct StationId3Frame final : EventData
{
	explicit StationId3Frame()
		: EventData(STATION_ID3)
	{
	}

	NRSC5::ID3 id3;
};

class HybridController final
{
	public:
		explicit HybridController();
		~HybridController();

		tl::expected<void, NRSC5::StreamStatus> Open(const NRSC5::StreamSupported &input);

		void Reset(double freq);
		void SetMode(Band::Type mode);
		void ProcessSamples(PortSDR::SDRTransfer &sdr_transfer);

		std::function<void(const NRSC5::Lot &lot)> onLotReceived;
		std::function<void(const std::unique_ptr<StationAudioFrame> &)> onAudioFrame;
		std::function<void(const std::unique_ptr<EventData> &)> onEvent;

	private:
		static void NRSC5Callback(const nrsc5_event_t *evt, void *opaque);

		std::unique_ptr<nrsc5_t, decltype(&nrsc5_close)> nrsc5_decoder_;

		TunerMode tuner_mode_ = TunerMode::Empty;

		std::mutex mutex_;
		NRSC5::Processor stream_processor_;
		std::queue<PortSDR::SDRTransfer> buffer_;
		std::thread process_thread;
		std::atomic<bool> stop_ = false;
};

#endif //HYBRIDTUNER_H
