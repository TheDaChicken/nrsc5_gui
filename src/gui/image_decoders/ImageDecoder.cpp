//
// Created by TheDaChicken on 7/13/2025.
//

#include "ImageDecoder.h"

#include <fstream>

#include "PNGDecoder.h"
#include "SVGDecoder.h"
#include "JPEGDecoder.h"

#ifdef _WINUSER_
#undef LoadImage
#endif

#include "utils/Log.h"

GUI::FileLoadResult GUI::ImageDecoder::LoadImageFromFile(
	const std::filesystem::path &filePath,
	ImageBuffer &image,
	const FileType type)
{
	if (filePath.empty())
		return InvalidResourceName;

	FileType expected_type = type;
	if (expected_type == FileType::Unknown)
		expected_type = DetectTypeFromExt(filePath);

	std::ifstream file(filePath, std::ios::binary);
	return LoadImage(file, image, expected_type);
}

GUI::FileLoadResult GUI::ImageDecoder::LoadImageFromData(
	const std::string& data,
	ImageBuffer &image,
	const FileType type)
{
	std::istringstream file(data, std::ios::binary);
	return LoadImage(file, image, type);
}

GUI::FileLoadResult GUI::ImageDecoder::LoadImage(std::istream& stream, ImageBuffer &image, FileType type)
{
	FileType expected_type = type;
	FileLoadResult result;

	// Load the image based on its type
	if (expected_type == FileType::PNG)
	{
		PNGDecoder png_decoder;
		result = png_decoder.Decode(stream, image);
	}
	if (expected_type == FileType::JPEG)
	{
		JPEGDecoder jpeg_decoder;
		result = jpeg_decoder.Decode(stream, image);
	}
	if (expected_type == FileType::SVG)
	{
		SVGDecoder svg_decoder;
		result = svg_decoder.Decode(stream);
		if (result == Success)
			svg_decoder.Render(image);
	}

	return result;
}

GUI::FileType GUI::ImageDecoder::DetectTypeFromExt(const std::filesystem::path &filePath)
{
	if (filePath.empty())
		return FileType::Unknown;

	const std::string ext = filePath.extension().string();
	if (ext == ".png")
		return FileType::PNG;
	if (ext == ".jpg" || ext == ".jpeg")
		return FileType::JPEG;
	if (ext == ".svg")
		return FileType::SVG;

	return FileType::Unknown;
}
