//
// Created by TheDaChicken on 8/1/2025.
//

#include "ListView.h"

bool ListView::RenderItem(
	const Item &item,
	const bool selected)
{
	bool changed = false;
	const char *view_text = item.name.data();
	const float icon_size = ImGui::GetFontSize() * 2.0f;

	const ImVec2 content_size = {
		ImGui::GetContentRegionAvail().x > 100 ? ImGui::GetContentRegionAvail().x : 100,
		icon_size + ImGui::GetStyle().FramePadding.y * 2.0f
	};

	if (ImGui::InvisibleButton(view_text, content_size))
		changed = true;

	ImDrawList *draw_list = ImGui::GetWindowDrawList();

	const ImVec2 pos_max = ImGui::GetItemRectMax();
	ImVec2 pos = ImGui::GetItemRectMin();

	const bool highlighted = ImGui::IsItemHovered();
	const bool out_held = ImGui::IsItemActive() && ImGui::IsMouseReleased(ImGuiMouseButton_Left);

	if (selected || highlighted)
	{
		const ImU32 col = ImGui::GetColorU32(
			(out_held && highlighted)
				? ImGuiCol_HeaderActive
				: highlighted
					  ? ImGuiCol_HeaderHovered
					  : ImGuiCol_Header);

		draw_list->AddRectFilled(
			pos,
			pos_max,
			col,
			ImGui::GetStyle().FrameRounding,
			ImDrawFlags_RoundCornersAll);
	}

	pos.x += ImGui::GetStyle().FramePadding.x;

	const ImVec2 icon_space = {icon_size, icon_size};
	const ImVec2 text_size = ImGui::CalcTextSize(view_text);

	if (item.image)
	{
		const float image_center = (content_size.y - icon_space.y) / 2.0f;

		draw_list->AddImage(item.image->GetPtr(),
		                    ImVec2{pos.x, pos.y + image_center},
		                    ImVec2{
			                    pos.x + icon_space.x,
			                    pos.y + image_center + icon_space.y
		                    });
		pos.x += icon_space.x;
	}

	pos.x += ImGui::GetStyle().ItemInnerSpacing.x;

	const float text_center = (content_size.y - text_size.y) / 2.0f;

	draw_list->AddText({pos.x, pos.y + text_center},
	                   ImGui::GetColorU32(ImGuiCol_Text),
	                   view_text);
	return changed;
}
