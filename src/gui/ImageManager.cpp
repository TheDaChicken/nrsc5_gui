//
// Created by TheDaChicken on 7/13/2025.
//

#include "ImageManager.h"

#include <array>
#include <fstream>

#include "utils/Log.h"

GUI::ImageManager::ImageManager()
{
}

GUI::FileLoadResult GUI::ImageManager::LoadImage(
	const std::string &filePath,
	Image &image,
	const Image::Type type)
{
	if (filePath.empty())
		return InvalidResourceName;

	std::ifstream file(filePath, std::ios::binary);

	// Load the image based on its type
	if (type == Image::Type::SVG)
	{
		image.type = Image::Type::SVG;
		// Load SVG image
		return m_svgDecoder.Decode(file, image);
	}
	if (type == Image::Type::PNG)
	{
		image.type = Image::Type::PNG;
		// Load PNG image
		if (PNGDecoder::Detect(file))
		{
			return m_pngDecoder.Decode(file, image);
		}
		return UnsupportedFormat;
	}

	// Auto-detect the image type
	if (PNGDecoder::Detect(file))
	{
		image.type = Image::Type::PNG;
		return m_pngDecoder.Decode(file, image);
	}
	if (SVGDecoder::Detect(file))
	{
		image.type = Image::Type::SVG;
		return m_svgDecoder.Decode(file, image);
	}

	return UnsupportedFormat;
}
