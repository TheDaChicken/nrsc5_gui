//
// Created by TheDaChicken on 7/27/2025.
//

#ifndef ICON_H
#define ICON_H

#include <cmath>
#include <filesystem>
#include <fstream>

#include "gui/image_decoders/ImageBuffer.h"
#include "gui/image_decoders/SVGDecoder.h"
#include "gui/wrappers/Texture.h"
#include "gui/wrappers/TextureUploader.h"

namespace GUI
{
struct SVGImage
{
	explicit SVGImage(const std::shared_ptr<GPU::TextureUploader> &uploader)
		: uploader_(uploader),
		  texture(uploader->CreateTexture())
	{
	}

	FileLoadResult Load(const std::filesystem::path &fullPath)
	{
		path = fullPath;

		std::ifstream svgFile(path);
		if (!svgFile.is_open())
			return FileNotFound;

		return decoder.Decode(svgFile);
	}

	GPU::Texture &UpdateSize(const float height)
	{
		if (!decoder)
			return texture;

		const int h = std::ceil(height);
		if (texture.height != h)
		{
			ImageBuffer data;
			data.uri = path.string();
			data.type = FileType::SVG;
			data.height = h;

			decoder.Render(data);
			uploader_->LoadImage(texture, data);
		}
		return texture;
	}

	[[nodiscard]] GPU::Texture &GetTexture()
	{
		return texture;
	}

	GPU::Texture &GetTextureByFontHeight()
	{
		return UpdateSize(ImGui::GetFontSize());
	}

	ImVec2 GetInlineSize()
	{
		UpdateSize(GlyphSize());
		return {static_cast<float>(texture.width), ImGui::GetFontSize()};
	}

	void PlaceInLineText()
	{
		ImGui::Dummy(GetInlineSize());
		DrawInline(ImGui::GetItemRectMin());
	}

	void DrawInline(const ImVec2& p)
	{
		UpdateSize(GlyphSize());
		if (!texture.ptr)
			return;

		const ImFontBaked *font_baked = ImGui::GetFontBaked();
		const float baseline = p.y + font_baked->Ascent;

		const ImVec2 p1(p.x, baseline - texture.height);
		const ImVec2 p2(p.x + texture.width, baseline);

		ImGui::GetWindowDrawList()->AddImage((intptr_t)texture.ptr.get(), p1, p2);
	}

	SVGDecoder decoder;
	std::filesystem::path path;
	std::shared_ptr<GPU::TextureUploader> uploader_;
	GPU::Texture texture;

	private:
		[[nodiscard]] float GlyphSize(char refGlyph = 'R')
		{
			ImFontBaked *font_baked = ImGui::GetFontBaked();
			const ImFontGlyph *glyph = font_baked->FindGlyph(refGlyph);
			float glyph_height;
			if (!glyph)
			{
				glyph_height = font_baked->Ascent - font_baked->Descent;
			}
			else
			{
				// Height using glyph's bounding box from baseline
				glyph_height = glyph->Y1 - glyph->Y0;
			}
			return glyph_height;
		}
};
}

#endif //ICON_H
