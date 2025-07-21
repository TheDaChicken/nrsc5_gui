//
// Created by TheDaChicken on 7/14/2025.
//

#include "SVGDecoder.h"

#include <array>
#include <istream>
#include <lunasvg.h>

GUI::FileLoadResult SVGDecoder::Decode(std::istream &in, GUI::Image &image)
{
	in.seekg(0);

	std::string content;
	std::getline(in, content, '\0'); // Read until EOF

	auto document = lunasvg::Document::loadFromData(content.c_str(), content.size());
	if (!document)
		return GUI::FileLoadResult::DecodingError;

	auto bitmap = document->renderToBitmap(image.width, image.height);
	bitmap.convertToRGBA();

	image.width = bitmap.width();
	image.height = bitmap.height();
	image.color_type = GUI::Image::ColorType::RGBA;
	image.bit_depth = GUI::Image::BitDepth::Bit8;
	image.data.resize(image.width * image.height * 4); // RGBA has 4 channels

	memcpy(image.data.data(), bitmap.data(), image.data.size());
	return GUI::FileLoadResult::Success;
}

bool SVGDecoder::Detect(std::istream &in)
{
	bool result = false;
	char c;
	std::array<char, 3> tag_name{};

	while (in)
	{
		if (!in.get(c))
			break;

		if (c == '<')
		{
			if (!in.read(tag_name.data(), 3))
				break;

			if (tag_name[0] == 's' && tag_name[1] == 'v' && tag_name[2] == 'g')
			{
				result = true;
				break;
			}
		}
	}

	in.seekg(0);
	return result;
}
