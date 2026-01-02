//
// Created by TheDaChicken on 11/22/2025.
//

#include "Decoder.h"

extern "C" {
#include <nrsc5.h>
}

static NRSC5::nrsc5_ptr nrsc5_ptr_open_pipe()
{
	nrsc5_t *st;
	// This won't happen. The function always returns 0.
	if (const int ret = nrsc5_open_pipe(&st); ret < 0)
		throw std::runtime_error("Failed to open NRSC5 pipe");

	return {st, nrsc5_close};
}

int ConvertToNRSC5Mode(const Band::Type mode)
{
	switch (mode)
	{
		case Band::FM:
			return NRSC5_MODE_FM;
		case Band::AM:
			return NRSC5_MODE_AM;
		default:
			return NRSC5_MODE_FM;
	}
}

NRSC5::Decoder::Decoder()
	: decoder_(nrsc5_ptr_open_pipe())
{
}

int NRSC5::Decoder::Reset(const float freq) const
{
	return nrsc5_set_frequency(
		decoder_.get(),
		freq);
}

void NRSC5::Decoder::SetCallback(const nrsc5_callback_t callback, void *opaque) const
{
	nrsc5_set_callback(
		decoder_.get(),
		callback,
		opaque);
}

void NRSC5::Decoder::SetMode(const Band::Type mode) const
{
	nrsc5_set_mode(
		decoder_.get(),
		ConvertToNRSC5Mode(mode));
}

void NRSC5::Decoder::PipeSamplesCS16(const int16_t *samples, const unsigned int length) const
{
	nrsc5_pipe_samples_cs16(
		decoder_.get(),
		samples,
		length);
}

void NRSC5::Decoder::PipeSamplesCU8(const uint8_t *samples, unsigned int length) const
{
	nrsc5_pipe_samples_cu8(
		decoder_.get(),
		samples,
		length);
}
