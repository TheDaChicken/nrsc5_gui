//
// Created by TheDaChicken on 7/22/2025.
//

#ifndef JPEGDECODER_H
#define JPEGDECODER_H

#include <cstdio>
#include <csetjmp>
#include <jpeglib.h>

#include "ImageBuffer.h"

class JPEGDecoder
{
	public:
		JPEGDecoder();
		~JPEGDecoder();

		GUI::FileLoadResult Decode(std::istream &in, GUI::ImageBuffer &image);
		GUI::FileLoadResult Decode(std::string_view in, GUI::ImageBuffer &image);

	private:
		jpeg_decompress_struct cinfo;
};

#endif //JPEGDECODER_H
