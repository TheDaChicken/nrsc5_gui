//
// Created by TheDaChicken on 11/22/2025.
//

#ifndef NRSC5_GUI_DECODER_H
#define NRSC5_GUI_DECODER_H

#include "utils/Band.h"
#include <string_view>

extern "C" {
typedef struct nrsc5_event_t nrsc5_event_t;

typedef void (*nrsc5_callback_t)(const nrsc5_event_t *evt, void *opaque);
typedef struct nrsc5_t nrsc5_t;
void nrsc5_close(nrsc5_t *st);
void nrsc5_program_type_name(unsigned int type, const char **name);
}

namespace NRSC5
{
using nrsc5_ptr = std::unique_ptr<nrsc5_t, decltype(&nrsc5_close)>;

class Decoder
{
	public:
		static std::string_view GetProgramTypeName(const unsigned int type)
		{
			const char *name;
			nrsc5_program_type_name(type, &name);
			return name;
		}

		Decoder();

		int Reset(float freq) const;

		void SetCallback(nrsc5_callback_t callback, void *opaque) const;
		void SetMode(Band::Type mode) const;
		void PipeSamplesCS16(const int16_t *samples, unsigned int length) const;
		void PipeSamplesCU8(const uint8_t *samples, unsigned int length) const;

	private:
		nrsc5_ptr decoder_;
};
} // namespace NRSC5

#endif //NRSC5_GUI_DECODER_H
