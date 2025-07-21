//
// Created by TheDaChicken on 7/14/2025.
//

#ifndef IMAGE_H
#define IMAGE_H

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

struct Image
{
	enum class Type
	{
		SVG,
		PNG,
		JPEG,
		Unknown
	};

	enum class ColorType
	{
		Unknown = -1,
		RGB,
		RGBA,
		Grayscale,
	};

	enum class BitDepth
	{
		Unknown = -1,
		Bit2 = 2,
		Bit4 = 4,
		Bit8 = 8,
		Bit16 = 16,
	};

	Type type = Type::Unknown;
	int width = -1;
	int height = -1;
	BitDepth bit_depth = BitDepth::Unknown;
	ColorType color_type = ColorType::Unknown;

	std::vector<unsigned char> data;
};
}

#endif //IMAGE_H
