//
// Created by TheDaChicken on 7/13/2025.
//

#ifndef IMAGE_MANAGER_H
#define IMAGE_MANAGER_H

#include <filesystem>
#include <string>
#include <vector>

#include "image/PNGDecoder.h"
#include "image/SVGDecoder.h"

namespace GUI
{
class ImageManager
{
	public:
		ImageManager();

		FileLoadResult LoadImage(
			const std::string &filePath,
			Image &image,
			Image::Type type = Image::Type::Unknown
		);

	private:
		PNGDecoder m_pngDecoder;
		SVGDecoder m_svgDecoder;
};
}

#endif //IMAGE_MANAGER_H
