//
// Created by TheDaChicken on 7/14/2025.
//

#include "SVGDecoder.h"

#include <array>
#include <istream>
#include <lunasvg.h>

GUI::FileLoadResult SVGDecoder::Decode(std::istream &in)
{
	in.seekg(0);

	std::string content;
	std::getline(in, content, '\0'); // Read until EOF
	return Decode(content);
}

GUI::FileLoadResult SVGDecoder::Decode(const std::string_view in)
{
	document = lunasvg::Document::loadFromData(in.data(), in.size());
	if (!document)
		return GUI::FileLoadResult::DecodingError;
	return GUI::FileLoadResult::Success;
}

void SVGDecoder::Render(GUI::ImageBuffer &image) const
{
	auto bitmap = document->renderToBitmap(image.width, image.height);
	bitmap.convertToRGBA();

	image.type = GUI::FileType::SVG;
	image.width = bitmap.width();
	image.height = bitmap.height();
	image.color_type = GUI::ImageBuffer::ColorType::RGBA;
	image.data.resize(image.width * image.height * 4); // RGBA has 4 channels

	memcpy(image.data.data(), bitmap.data(), image.data.size());
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
