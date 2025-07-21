//
// Created by TheDaChicken on 7/14/2025.
//

#ifndef SVGDECODER_H
#define SVGDECODER_H

#include "Image.h"

class SVGDecoder {
	public:
		GUI::FileLoadResult Decode(std::istream &in, GUI::Image &image);

		static bool Detect(std::istream &in);
};

#endif //SVGDECODER_H
