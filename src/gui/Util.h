//
// Created by TheDaChicken on 9/27/2025.
//

#ifndef GUI_UTIL_H
#define GUI_UTIL_H

#include <imgui.h>

#include "utils/Icon.h"

inline float Center(const float window, const float widget)
{
	return std::max(0.0f, (window - widget) * 0.5f);
}

[[nodiscard]] inline float GlyphSize(const char refGlyph = 'R')
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

inline ImVec2 GetInlineSize(GUI::SVGImage &image)
{
	const auto &texture = image.UpdateSize(GlyphSize());
	return {static_cast<float>(texture.width), ImGui::GetFontSize()};
}

inline void DrawInline(GUI::SVGImage &image, const ImVec2 &p)
{
	const auto &texture = image.UpdateSize(GlyphSize());
	if (!texture.ptr)
		return;

	const ImFontBaked *font_baked = ImGui::GetFontBaked();
	const float baseline = p.y + font_baked->Ascent;

	const ImVec2 p1(p.x, baseline - texture.height);
	const ImVec2 p2(p.x + texture.width, baseline);

	ImGui::GetWindowDrawList()->AddImage((intptr_t)texture.ptr.get(), p1, p2);
}

#endif //UTIL_H
