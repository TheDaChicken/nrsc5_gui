//
// Created by TheDaChicken on 7/13/2025.
//

#ifndef PNGDECODER_H
#define PNGDECODER_H

#include <png.h>

#include <istream>
#include "Image.h"

class PNGDecoder
{
	public:
		PNGDecoder();
		~PNGDecoder();

		GUI::FileLoadResult Decode(std::istream &in, GUI::Image &image);

		static bool Detect(std::istream &in);

		static GUI::Image::ColorType ColorType(png_byte color_type);
		GUI::Image::BitDepth BitDepth(png_byte color_type);

	private:
		png_structp png_ptr_;
		png_infop info_ptr_;
};

#endif //PNGDECODER_H
