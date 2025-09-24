//
// Created by TheDaChicken on 7/14/2025.
//

#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <string>
#include <vector>

namespace GUI
{
enum FileLoadResult : int
{
	Success = 0,
	FileNotFound,
	FileReadError,
	UnsupportedFormat,
	DecodingError,
	InvalidResourceName,
};

enum class FileType
{
	SVG,
	PNG,
	JPEG,
	Unknown
};

struct ImageBuffer
{
	enum class ColorType
	{
		Unknown = -1,
		RGB,
		RGBA
	};

	std::string uri = "unknown";

	FileType type = FileType::Unknown;
	int width = -1;
	int height = -1;
	ColorType color_type = ColorType::Unknown;

	std::vector<unsigned char> data;
};
}

#endif //IMAGEDATA_H
