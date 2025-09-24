//
// Created by TheDaChicken on 7/13/2025.
//

#ifndef IMAGE_MANAGER_H
#define IMAGE_MANAGER_H

#include <filesystem>

#include "ImageBuffer.h"

namespace GUI
{
class ImageDecoder
{
	public:
		static FileLoadResult LoadImageFromFile(
			const std::filesystem::path &filePath,
			ImageBuffer &image,
			FileType type = FileType::Unknown
		);

		static FileLoadResult LoadImageFromData(
			const std::string& data,
			ImageBuffer &image,
			FileType type = FileType::Unknown
		);

		static FileLoadResult LoadImage(
			std::istream& stream,
			ImageBuffer &image,
			FileType type = FileType::Unknown
		);

		static FileType DetectTypeFromExt(const std::filesystem::path &filePath);
};
}

#endif //IMAGE_MANAGER_H
