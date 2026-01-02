//
// Created by TheDaChicken on 9/27/2025.
//

#ifndef GUI_UTIL_H
#define GUI_UTIL_H

#include <imgui.h>
#include <imgui_internal.h>

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
	return {static_cast<float>(texture->GetWidth()), ImGui::GetFontSize()};
}

inline void DrawInline(GUI::SVGImage &image, const ImVec2 &p)
{
	const auto &texture = image.UpdateSize(GlyphSize());
	if (!texture->IsAllocated())
		return;

	const ImFontBaked *font_baked = ImGui::GetFontBaked();
	const float baseline = p.y + font_baked->Ascent;

	const ImVec2 p1(p.x, baseline - texture->GetHeight());
	const ImVec2 p2(p.x + texture->GetWidth(), baseline);

	ImGui::GetWindowDrawList()->AddImage(texture->GetPtr(), p1, p2);
}

inline bool SliderFloatWithSteps(
	const char *label, float *v, float v_min, float v_max, float v_step,
	const char *display_format = "&.3f")
{
	char text_buf[64] = {};
	ImFormatString(text_buf, IM_ARRAYSIZE(text_buf), display_format, *v);

	// Map from [v_min,v_max] to [0,N]
	const int countValues = static_cast<int>((v_max - v_min) / v_step);
	int v_i = static_cast<int>((*v - v_min) / v_step);
	const bool value_changed = ImGui::SliderInt(label, &v_i, 0,
		countValues, text_buf);

	// Remap from [0,N] to [v_min,v_max]
	*v = v_min + float(v_i) * v_step;
	return value_changed;
}

#endif //UTIL_H
