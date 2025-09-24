//
// Created by TheDaChicken on 7/13/2025.
//

#ifndef PNGDECODER_H
#define PNGDECODER_H

#include <png.h>

#include <istream>
#include "ImageBuffer.h"

class PNGDecoder
{
	public:
		PNGDecoder();
		~PNGDecoder();

		GUI::FileLoadResult Decode(std::istream &in, GUI::ImageBuffer &image);
		//GUI::FileLoadResult Decode(std::string_view in, GUI::ImageBuffer &image);

		static bool Detect(std::istream &in);
		static bool Detect(std::string_view in);

	private:
		GUI::FileLoadResult TryDecode(GUI::ImageBuffer &image);

		png_structp png_ptr_;
		png_infop info_ptr_;
};

#endif //PNGDECODER_H
