//
// Created by TheDaChicken on 9/26/2023.
//

#ifndef NRSC5_GUI_STATIONITEM_H
#define NRSC5_GUI_STATIONITEM_H

#include <utility>

#include "nrsc5/Station.h"
#include "Modulation.h"

#include <nlohmann/json.hpp>
#include <utils/Log.h>

struct TunerOpts
{
	TunerOpts() = default;
	TunerOpts(const Modulation::Type &type, const uint32_t frequency)
		: type(type), freq(frequency)
	{
	}
	TunerOpts(const Modulation::Type type, const double frequency)
		: type(type)
	{
		const Modulation &modulation = GetModulation(type);
		const int pow = modulation.decimal_places * 10;

		assert(frequency > modulation.min && frequency < modulation.max);
		freq = static_cast<uint32_t>(round(frequency * pow)) * modulation.scale / pow;
	}

	Modulation::Type type{Modulation::Type::MOD_FM};
	uint32_t freq{0};

	[[nodiscard]] double GetFrequencyInShort() const
	{
		const Modulation &modulation = GetModulation(type);
		return static_cast<double>(freq) / modulation.scale;
	}

	bool operator==(const TunerOpts &other) const
	{
		return type == other.type && freq == other.freq;
	}

	private:
		NLOHMANN_DEFINE_TYPE_INTRUSIVE(TunerOpts, type, freq)
};

struct Channel
{
	Channel() = default;

	Channel(
		const TunerOpts tunerData,
		const unsigned int programId)
		: tuner_opts(tunerData)
	{
		station_info.current_program = programId;
	}

	Channel(const TunerOpts tunerData,
	        NRSC5::StationInfo stationInfo)
		: tuner_opts(std::move(tunerData)),
		  station_info(std::move(stationInfo))
	{
	}

	TunerOpts tuner_opts;
	NRSC5::StationInfo station_info;

	bool operator==(const Channel &other) const
	{
		return tuner_opts == other.tuner_opts && station_info == other.station_info;
	}

	private:
		NLOHMANN_DEFINE_TYPE_INTRUSIVE(Channel, tuner_opts, station_info)
};

struct ActiveChannel : Channel
{
	ActiveChannel() = default;

	ActiveChannel(
		TunerOpts tunerData,
		NRSC5::StationInfo stationInfo,
		NRSC5::StationDetails stationDetails)
		: Channel(tunerData, std::move(stationInfo)),
		  hd_details(std::move(stationDetails))
	{
	}

	NRSC5::StationDetails hd_details;
};

#endif //NRSC5_GUI_STATIONITEM_H
