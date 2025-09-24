//
// Created by TheDaChicken on 7/18/2025.
//

#ifndef ASSETSMANAGER_H
#define ASSETSMANAGER_H

#include <filesystem>
#include <optional>

#include "gui/image_decoders/ImageBuffer.h"
#include "utils/Expected.hpp"

namespace GUI
{
class AssetsManager
{
	public:
		AssetsManager();

		bool HasAssetsFolder();

		[[nodiscard]] std::optional<std::filesystem::path> GetAssetsFolder() const
		{
			return assetsFolder_;
		}

		[[nodiscard]] tl::expected<std::filesystem::path, FileLoadResult> GetResource(
			std::string_view resource_name
		) const;

	private:
		[[nodiscard]] static std::optional<std::filesystem::path> TryAssetsFolder();
		static std::optional<std::filesystem::path> GetExecutableFolder();

		std::optional<std::filesystem::path> assetsFolder_;
};
} // namespace GUI

#endif //ASSETSMANAGER_H
