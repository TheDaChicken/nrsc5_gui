//
// Created by TheDaChicken on 7/30/2025.
//

#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <array>
#include <unordered_map>

#include "AssetsManager.h"
#include "ImageManager.h"
#include "gui/utils/Icon.h"

enum class FontType : int
{
	Semibold = 0,
	Bold,

	COUNT,
};

enum class IconType : int
{
	UNKNOWN = -1,
	Play = 0,
	Paused,
	Note,
	Navigation,
	Settings,
	MinusCircle,
	Plus,
	Minus,
	Back,
	HDLogo,

	COUNT,
};

enum class ImageType : int
{
	ChannelDefault = 0,
	PrimaryDefault,

	COUNT,
};

struct ThemeOptions
{
	std::string name;

	ImGuiStyle style;

	float navigation_x;
	float list_item_multiplier;
	float separator_thickness;

	float font_small_size;
	float font_medium_size;
	float font_large_size;

	std::array<std::string, static_cast<int>(FontType::COUNT)> fonts;
	std::array<std::string, static_cast<int>(IconType::COUNT)> icons;
	std::array<std::string, static_cast<int>(ImageType::COUNT)> images;
};

struct Theme
{
	std::string name;

	ImGuiStyle style{};

	float font_small_size;
	float font_medium_size;
	float font_large_size;

	float navigation_x;
	float list_item_multiplier;
	float separator_thickness;

	std::array<ImFont *, static_cast<int>(FontType::COUNT)> fonts;
	std::array<std::shared_ptr<GUI::SVGImage>, static_cast<int>(IconType::COUNT)> icons = {};
	std::array<std::shared_ptr<GPU::Texture>, static_cast<int>(ImageType::COUNT)> images = {};

	[[nodiscard]] std::shared_ptr<GUI::SVGImage> GetIcon(IconType type) const
	{
		if (type == IconType::UNKNOWN)
			return nullptr;

		const int index = static_cast<int>(type);
		if (index < 0 || index >= icons.size())
			throw std::out_of_range("Icon type index out of range");
		return icons[index];
	}

	[[nodiscard]] const std::shared_ptr<GPU::Texture>& GetImage(ImageType type) const
	{
		const int index = static_cast<int>(type);
		if (index < 0 || index >= icons.size())
			throw std::out_of_range("Image type index out of range");
		return images[index];
	}

	[[nodiscard]] ImFont *GetFont(FontType type) const
	{
		const int index = static_cast<int>(type);
		if (index < 0 || index >= icons.size())
			throw std::out_of_range("Image type index out of range");
		return fonts[index];
	}
};

struct TextEnter
{

	std::string line;
};

class ThemeManager
{
	public:
		explicit ThemeManager(std::shared_ptr<GPU::TextureUploader> device)
			: uploader_(std::move(device))
		{
		}

		static void Separator(float thickness = 1.0f);

		bool Init();
		bool LoadTheme(const ThemeOptions &opt);

		bool SetCurrentTheme(const std::string &theme_name);
		void ApplyTheme(const Theme &theme);

		const Theme &GetCurrentTheme() const
		{
			return current_theme_;
		}

	private:
		static ThemeOptions GetLightTheme();

		std::shared_ptr<GPU::TextureUploader> uploader_;

		Cache<std::string, GUI::SVGImage> svg_cache_;
		Cache<std::string, GPU::Texture> images_cache_;

		std::unordered_map<std::string, Theme> themes_;
		Theme current_theme_ = {};

		GUI::AssetsManager assets_manager_;
};

#endif //STYLEMANAGER_H
