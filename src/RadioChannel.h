//
// Created by TheDaChicken on 9/26/2023.
//

#ifndef NRSC5_GUI_STATIONITEM_H
#define NRSC5_GUI_STATIONITEM_H

#include "nrsc5/Station.h"
#include "utils/Band.h"

#include <nlohmann/json.hpp>

#include "gui/managers/ImageManager.h"

enum class MimeQuery
{
	StationLogo,
	PrimaryLogo
};

struct MetadataQuery
{
	std::string country_code;
	std::string name;

	MimeQuery type;
	unsigned int program_id = 0;
	int xhr_id = 0;
};

struct StationIdentity
{
	unsigned int id{0};
	std::string country_code;
	std::string name;
};

struct Station
{
	uint32_t freq{0};
	Band::Type mode{Band::Type::FM};
	unsigned int program_id{0}; // HD program ID, 0 for analog

	StationIdentity identity;

	[[nodiscard]] std::string GetShort() const
	{
		std::string text = Band::FormatFrequency(mode, freq);
		if (program_id > 0)
			text += fmt::format(" HD{}", NRSC5::FriendlyProgramId(program_id));
		return text;
	}
};

inline void to_json(nlohmann::json &j, const Station &p)
{
	j = nlohmann::json{
		{"mode", p.mode},
		{"freq", p.freq},
		{"station_id", p.identity.id},
		{"country_code", p.identity.country_code},
		{"name", p.identity.name},
		{"program_id", p.program_id},
	};
}

inline void from_json(const nlohmann::json &j, Station &p)
{
	// Check for old version compatibility
	if (j.contains("tuner_opts"))
	{
		j.at("tuner_opts").at("type").get_to(p.mode);
		j.at("tuner_opts").at("freq").get_to(p.freq);
	}
	else
	{
		j.at("mode").get_to(p.mode);
		j.at("freq").get_to(p.freq);
	}
	if (j.contains("station_info"))
	{
		j.at("station_info").at("country_code").get_to(p.identity.country_code);
		j.at("station_info").at("id").get_to(p.identity.id);
		j.at("station_info").at("name").get_to(p.identity.name);
		j.at("station_info").at("current_program").get_to(p.program_id);
	}
	else
	{
		j.at("station_id").get_to(p.identity.id);
		j.at("country_code").get_to(p.identity.country_code);
		j.at("name").get_to(p.identity.name);
		j.at("program_id").get_to(p.program_id);
	}
}

#endif //NRSC5_GUI_STATIONITEM_H
