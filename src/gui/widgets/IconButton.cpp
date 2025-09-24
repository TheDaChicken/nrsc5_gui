//
// Created by TheDaChicken on 7/13/2025.
//

#include "IconButton.h"

#include <imgui.h>

#include "utils/Log.h"

bool GUI::IconButton::Render(const bool checked) const
{
	ImDrawList *draw_list = ImGui::GetWindowDrawList();
	bool updated = false;

	if (!icon)
		return false;

	const GPU::Texture &image = icon->GetTextureByFontHeight();

	if (ImGui::InvisibleButton(id.c_str(),
	                           {
		                           static_cast<float>(image.height * 2),
		                           static_cast<float>(image.height * 2)
	                           }))
		updated = true;

	const ImVec2 center_pos(
		ImGui::GetItemRectMin().x + ImGui::GetItemRectSize().x * 0.5f,
		ImGui::GetItemRectMin().y + ImGui::GetItemRectSize().x * 0.5f);
	const float diameter = ImGui::GetItemRectSize().y;
	const float radius = 0.5f * diameter;

	if (checked)
	{
		draw_list->AddCircleFilled(
			center_pos,
			radius,
			checkedColor,
			0);
	}

	draw_list->AddImage(
		(intptr_t)image.ptr.get(),
		ImVec2(center_pos.x - image.width * 0.5f, center_pos.y - image.height * 0.5f),
		ImVec2(center_pos.x + image.width * 0.5f, center_pos.y + image.height * 0.5f));
	return updated;
}
