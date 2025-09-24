//
// Created by TheDaChicken on 9/1/2025.
//

#ifndef HYBRIDSTATE_H
#define HYBRIDSTATE_H

#include "RadioChannel.h"
#include "utils/RingBuffer.h"

struct ProgramState
{
	unsigned int id{0};
	unsigned int type = NRSC5_PROGRAM_TYPE_UNDEFINED;

	std::string name;

	TextureHandle station_logo;
	TextureHandle primary_image;
	NRSC5::ID3 id3;
};

struct HybridState
{
	HybridState() = default;

	uint32_t freq{0};
	Band::Type mode{Band::Type::FM};

	unsigned int id{0};

	std::string country_code;
	std::string name;

	std::string frequency_text;
	std::string message;
	std::string slogan;

	void Reset()
	{
		*this = HybridState();
	}

	bool IsDry()
	{
		return country_code.empty() || name.empty();
	}
};

#endif //HYBRIDSTATE_H
