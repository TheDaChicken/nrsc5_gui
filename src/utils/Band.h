#ifndef BAND_H
#define BAND_H

#include <string>
#include <fmt/format.h>

namespace Band
{
enum Type
{
	AM = 0,
	FM = 1,
};

struct Info
{
	double minFrequency = 0;
	double maxFrequency = 0;
	double step = 0;
	float scale = 0;
	int decimal_places;
	std::string label = "Unknown";
};

inline Info GetInfo(const Type band)
{
	switch (band)
	{
		case FM: return {87.5e6, 108.0e6, 100e3, 1e6, 1, "FM"};
		case AM: return {520e3, 1710e3, 10e3, 1e3, 0, "AM"};
		default: return {0, 0, 1e3, 0, 0, "Unknown"};
	}
}

inline std::string FormatFrequency(const Type band, const uint32_t frequency)
{
	const Info info = GetInfo(band);
	if (frequency < info.minFrequency || frequency > info.maxFrequency)
		return "Out of range";

	return fmt::format("{:.{}f}", frequency / info.scale, info.decimal_places);
}

};

#endif //BAND_H
