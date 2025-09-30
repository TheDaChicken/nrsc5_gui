//
// Created by TheDaChicken on 7/30/2025.
//

#include "ThemeManager.h"

#include "gui/image_decoders/ImageDecoder.h"
#include "utils/Log.h"

std::unordered_map<std::string, ImFont *> fonts;

static ImFont *GetOrLoadFont(const std::filesystem::path &path, const float font_size, const ImFontConfig *font_cfg_template)
{
	const std::string name = path.filename().string();

	const auto iter = fonts.find(name);
	if (iter != fonts.end())
		return iter->second;

	const std::string path_str = path.string();

	// Load new font
	ImFont *font = ImGui::GetIO().Fonts->AddFontFromFileTTF(
		path_str.c_str(),
		font_size,
		font_cfg_template);
	if (!font)
	{
		Logger::Log(err, "Failed to load font '{}'", path_str);
		return nullptr;
	}

	fonts[name] = font;
	return font;
}

void ThemeManager::Separator(const float thickness)
{
	const ImVec2 cursor = ImGui::GetCursorScreenPos();
	const float x2 = ImGui::GetContentRegionAvail().x;
	const ImVec2 size(0, thickness);

	ImGui::Dummy(size);
	ImDrawList *draw_list = ImGui::GetWindowDrawList();

	draw_list->AddRectFilled(
		ImVec2(cursor.x, cursor.y),
		ImVec2(cursor.x + x2, cursor.y + thickness),
		ImGui::GetColorU32(ImGuiCol_Separator));
}

bool ThemeManager::Init()
{
	if (!assets_manager_.HasAssetsFolder())
	{
		Logger::Log(err, "Theme assets cannot be loaded without assets folder.");
		return false;
	}

	const ThemeOptions light = GetLightTheme();
	if (!LoadTheme(light))
	{
		Logger::Log(err, "Failed to load light theme");
		return false;
	}

	return SetCurrentTheme("Light");
}

bool ThemeManager::LoadTheme(const ThemeOptions &opt)
{
	Theme theme;

	theme.style = opt.style;
	theme.name = opt.name;
	theme.list_item_multiplier = opt.list_item_multiplier;
	theme.separator_thickness = opt.separator_thickness;

	theme.font_small_size = opt.font_small_size;
	theme.font_medium_size = opt.font_medium_size;
	theme.font_large_size = opt.font_large_size;
	theme.font_very_large_size = opt.font_very_large_size;

	for (int i = 0; i < opt.fonts.size(); i++)
	{
		const auto &font_name = opt.fonts[i];

		auto font = assets_manager_.GetResource(font_name);
		if (!font)
		{
			Logger::Log(err,
			            "Failed to get semibold font path: {}",
			            static_cast<int>(font.error()));
			return false;
		}

		theme.fonts[i] = GetOrLoadFont(font.value(), theme.font_small_size, nullptr);
		if (!theme.fonts[i])
		{
			Logger::Log(err, "Failed to load bold font");
			return false;
		}
	}

	for (int i = 0; i < static_cast<int>(IconType::COUNT); ++i)
	{
		const auto &icon_name = opt.icons[i];
		auto icon_path = assets_manager_.GetResource(icon_name);
		if (!icon_path)
		{
			Logger::Log(err,
			            "Failed to get icon path for {}: {}",
			            icon_name,
			            static_cast<int>(icon_path.error()));
			return false;
		}

		auto [success, svg_image] = svg_cache_.TryInsert(icon_name, 1, uploader_);
		if (!svg_image)
		{
			Logger::Log(err,
			            "Failed to create SVG image for {}",
			            icon_name);
			return false;
		}

		if (success && svg_image->Load(icon_path.value()) != GUI::FileLoadResult::Success)
		{
			Logger::Log(err,
			            "Failed to load SVG image from path: {}",
			            icon_path->string());
			return false;
		}

		theme.icons[i] = svg_image;
	}

	GUI::ImageBuffer image_buffer;

	for (int i = 0; i < static_cast<int>(ImageType::COUNT); ++i)
	{
		const auto &image_name = opt.images[i];
		auto image_path = assets_manager_.GetResource(image_name);
		if (!image_path)
		{
			Logger::Log(err,
			            "Failed to get image path for {}: {}",
			            image_name,
			            static_cast<int>(image_path.error()));
			return false;
		}

		auto [success, texture] = images_cache_.TryInsert(image_name, 1, uploader_->CreateTexture());
		if (!texture)
		{
			Logger::Log(err,
			            "Failed to create texture for {}",
			            image_name);
			return false;
		}

		if (success)
		{
			if (GUI::ImageDecoder::LoadImageFromFile(image_path.value(), image_buffer) !=
				GUI::FileLoadResult::Success)
			{
				Logger::Log(err,
				            "Failed to load image from path: {}",
				            image_path->string());
				return false;
			}

			image_buffer.uri = image_path->string();

			uploader_->LoadImage(*texture, image_buffer);
		}

		theme.images[i] = texture;
	}

	themes_[theme.name] = theme;
	return true;
}

bool ThemeManager::SetCurrentTheme(const std::string &theme_name)
{
	const auto it = themes_.find(theme_name);
	if (it == themes_.end())
	{
		Logger::Log(err, "Theme '{}' not found", theme_name);
		return false;
	}

	ApplyTheme(it->second);
	return true;
}

void ThemeManager::ApplyTheme(const Theme &theme)
{
	const float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

	// Setup scaling
	ImGuiStyle &style = ImGui::GetStyle();

	style = theme.style;
	style.ScaleAllSizes(main_scale);
	// Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;
	// Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

	current_theme_ = theme;
}

ThemeOptions ThemeManager::GetLightTheme()
{
	ThemeOptions light{
		.name = "Light",
		.list_item_multiplier = 3.2f,
		.separator_thickness = 2.0f,
		.font_small_size = 22.0f,
		.font_medium_size = 25.0f,
		.font_large_size = 27.0f,
		.font_very_large_size = 34.0f,
		.fonts = {
			"fonts/OpenSans-SemiBold.ttf",
			"fonts/OpenSans-Bold.ttf"
		},
		.icons = {
			"buttons/black/Play.svg",
			"buttons/black/Pause.svg",
			"buttons/black/Note.svg",
			"buttons/black/Navigation.svg",
			"buttons/black/Settings.svg",
			"buttons/MinusCircle.svg",
			"buttons/black/Plus.svg",
			"buttons/black/Minus.svg",
			"buttons/blue/GreaterThan.svg",
			"images/HDRadioLogo.svg",
		},
		.images = {
			"images/BlackRadio.svg",
			"images/ImageMissing.jpg",
		},
	};

	light.style.FrameRounding = light.font_small_size / 2;
	light.style.WindowPadding = ImVec2{25, 16};
	light.style.FramePadding = ImVec2{12, 12};
	light.style.ItemSpacing = ImVec2{20, 15};
	light.style.ItemInnerSpacing = ImVec2{10, 15};
	light.style.CellPadding = ImVec2{4, 2};
	light.style.IndentSpacing = light.font_small_size + light.style.FramePadding.x * 2.0f;
	light.style.FrameBorderSize = 1.0f;

	// Base style
	ImGui::StyleColorsLight(&light.style);

	ImVec4 *colors = light.style.Colors;

	// Customize colors
	colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4(IM_COL32(10, 10, 10, 255));
	colors[ImGuiCol_TextDisabled] = ImGui::ColorConvertU32ToFloat4(IM_COL32(193, 193, 193, 255));
	colors[ImGuiCol_HeaderActive] = ImGui::ColorConvertU32ToFloat4(IM_COL32(178, 207, 255, 255));
	colors[ImGuiCol_Border] = ImGui::ColorConvertU32ToFloat4(IM_COL32(243, 243, 243, 255));
	colors[ImGuiCol_Separator] = ImGui::ColorConvertU32ToFloat4(IM_COL32(193, 193, 193, 255));
	colors[ImGuiCol_FrameBg] = ImGui::ColorConvertU32ToFloat4(IM_COL32(229, 229, 229, 255));
	colors[ImGuiCol_WindowBg] = ImGui::ColorConvertU32ToFloat4(IM_COL32(255, 255, 255, 255));

	return light;
}
