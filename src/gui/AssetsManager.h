//
// Created by TheDaChicken on 7/18/2025.
//

#ifndef ASSETSMANAGER_H
#define ASSETSMANAGER_H

#include <filesystem>

#include "ImageManager.h"
#include "image/Image.h"

namespace GUI
{
class AssetsManager
{
	public:
		explicit AssetsManager(ImageManager &imageManager)
			: imageManager_(imageManager)
		{
		}

		FileLoadResult LoadImageAsset(
			const std::string &resource_name,
			Image &image,
			Image::Type type = Image::Type::Unknown
		);

		static FileLoadResult GetAsset(
			const std::string &resource_name,
			std::filesystem::path &assetPath
		);

	private:
		[[nodiscard]] static std::filesystem::path GetAssetsFolder();
		static std::filesystem::path GetExecutableFolder();

		ImageManager &imageManager_;
};
} // namespace GUI

#endif //ASSETSMANAGER_H
