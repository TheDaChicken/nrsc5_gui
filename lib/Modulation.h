/**
 * @file Modulation.h
 * @brief Contains the Modulation class and constants defining types of modulation.
 */
#ifndef MODULATION_H
#define MODULATION_H

struct Modulation
{
	enum Type
	{
		MOD_AM = 0,
		MOD_FM = 1,
	} type;

	constexpr Modulation(
		const Type type,
		const double min,
		const double max,
		const double step,
		const short decimal_places,
		const int scale)
		: type(type), min(min), max(max), step(step), scale(scale), decimal_places(decimal_places)
	{
	}

	Modulation(const Modulation &other) = delete;
	Modulation &operator=(const Modulation &other) = delete;

	double min;
	double max;
	double step;

	// Scale to convert to Hz
	uint32_t scale;
	short decimal_places;
};

constexpr static auto MOD_FM = Modulation(Modulation::Type::MOD_FM, 87.5, 108.0, 0.2, 1, 1e6);
constexpr static auto MOD_AM = Modulation(Modulation::Type::MOD_AM, 520, 1710, 10, 0, 1e3);

inline const Modulation &GetModulation(const Modulation::Type type)
{
	switch (type)
	{
		case Modulation::Type::MOD_AM:
			return MOD_AM;
		case Modulation::Type::MOD_FM:
		default:
			return MOD_FM;
	}
}

#endif //MODULATION_H
