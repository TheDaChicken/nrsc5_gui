//
// Created by TheDaChicken on 9/26/2023.
//

#ifndef NRSC5_GUI_STATIONITEM_H
#define NRSC5_GUI_STATIONITEM_H

#include <utility>
#include <QString>

#include "nrsc5/Decoder.h"
#include "nrsc5/Station.h"
#include "Modulation.h"

#include <nlohmann/json.hpp>
#include <utils/Log.h>

struct TunerOptions
{
	Modulation::Type modulation_type = Modulation::Type::MOD_FM;
	uint32_t frequency_ = 0;

	[[nodiscard]] double GetScaledFrequency() const
	{
		const Modulation &modulation = GetModulation(modulation_type);
		return static_cast<double>(frequency_) / modulation.scale;
	}

	/** Set the frequency using the scaled value */
	void SetScaledFrequency(const double frequency)
	{
		const Modulation &modulation = GetModulation(modulation_type);
		assert(frequency > modulation.min && frequency < modulation.max);

		frequency_ = static_cast<uint32_t>(round(frequency * (modulation.decimal_places * 10)))
				* modulation.scale / (modulation.decimal_places * 10);
	}

	bool operator==(const TunerOptions &other) const
	{
		return frequency_ == other.frequency_;
	}

	private:
		NLOHMANN_DEFINE_TYPE_INTRUSIVE(TunerOptions, modulation_type, frequency_)
};

/**
 * @brief RadioChannel
 * @details RadioChannel is a struct that holds the information of a radio channel.
 */
struct RadioChannel
{
	RadioChannel() = default;
	~RadioChannel() = default;

	RadioChannel(const Modulation::Type type,
	             const double frequency, const unsigned int programId)
	{
		tuner_options.modulation_type = type;
		tuner_options.SetScaledFrequency(frequency);
		hd_station_.current_program = programId;
	}

	RadioChannel(TunerOptions tunerData, NRSC5::Station station)
		: tuner_options(tunerData), hd_station_(std::move(station))
	{
	}

	TunerOptions tuner_options;

	// HD Radio station data (optional)
	NRSC5::Station hd_station_;

	[[nodiscard]] QString GetDisplayChannel() const
	{
		QString station_freq = QString::number(tuner_options.GetScaledFrequency(), 'g', 5);

		if (hd_station_.current_program > NRSC5_MPS_PROGRAM)
		{
			station_freq += QString(" HD%1").arg(NRSC5::FriendlyProgramId(hd_station_.current_program));
		}

		return station_freq;
	}

	bool operator==(const RadioChannel &other) const
	{
		return tuner_options == other.tuner_options
				// Not everything may be set for HD Radio
				// We only compare the current program and station ID
				&& hd_station_.IsSimilar(other.hd_station_);
	}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(RadioChannel, tuner_options, hd_station_)
};

#endif //NRSC5_GUI_STATIONITEM_H
