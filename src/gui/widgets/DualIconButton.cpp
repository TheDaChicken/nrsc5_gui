//
// Created by TheDaChicken on 7/25/2025.
//

#include "DualIconButton.h"

#include <imgui.h>
#include <spdlog/spdlog.h>

#include "gui/Util.h"

GUI::DualIconButton::Result GUI::DualIconButton::Render() const
{
	auto res = Result::None;

	const float text_size = ImGui::CalcTextSize(label.c_str()).x;
	const float left_icon = left.icon ? GetInlineSize(*left.icon).x : 0;
	const float right_icon = right.icon ? GetInlineSize(*right.icon).x : 0;
	float child_width = text_size + ImGui::GetStyle().FramePadding.x * 2.0f;

	if (left.icon)
		child_width += left_icon + ImGui::GetStyle().ItemSpacing.x;
	if (right.icon)
		child_width += right_icon + ImGui::GetStyle().ItemSpacing.x;

	ImDrawList *draw_list = ImGui::GetWindowDrawList();

	ImGui::InvisibleButton(button_id.c_str(),
	                       ImVec2(child_width, ImGui::GetFrameHeight()));
	ImVec2 pos = ImGui::GetItemRectMin();
	ImVec2 pos_max = ImGui::GetItemRectMax();

	draw_list->AddRect(
		pos,
		pos_max,
		ImGui::GetColorU32(ImGuiCol_Border),
		ImGui::GetStyle().FrameRounding);

	pos_max.x -= ImGui::GetStyle().FramePadding.x;
	pos_max.y -= ImGui::GetStyle().FramePadding.y;

	// Padding from left
	pos.x += ImGui::GetStyle().FramePadding.x;
	// Center vertically
	pos.y = pos.y + Center(ImGui::GetItemRectSize().y, ImGui::GetFontSize());

	const ImVec2 icon_left_min{pos};
	const ImVec2 icon_left_max{pos.x + left_icon, pos.y + ImGui::GetFontSize()};
	const ImVec2 icon_right_min{pos_max.x - right_icon, pos.y};
	const ImVec2 icon_right_max{pos_max.x, pos.y + ImGui::GetFontSize()};

	if (left.icon)
	{
		DrawInline(*left.icon, pos);

		pos.x += left_icon + ImGui::GetStyle().ItemSpacing.x;
	}

	draw_list->AddText(pos,
	                   ImGui::GetColorU32(ImGuiCol_Text),
	                   label.data(),
	                   label.data() + label.size());

	pos.x += text_size + ImGui::GetStyle().ItemSpacing.x;

	if (right.icon)
	{
		ImGui::SameLine();

		DrawInline(*right.icon, pos);
	}

	if (ImGui::IsItemClicked())
	{
		const ImVec2 mouse_pos = ImGui::GetIO().MousePos;
		if (mouse_pos.x >= icon_left_min.x && mouse_pos.x <= icon_left_max.x
			&& mouse_pos.y >= icon_left_min.y && mouse_pos.y <= icon_left_max.y)
		{
			res = Result::Left;
		}
		else if (mouse_pos.x >= icon_right_min.x && mouse_pos.x <= icon_right_max.x
			&& mouse_pos.y >= icon_right_min.y && mouse_pos.y <= icon_right_max.y)
		{
			res = Result::Right;
		}
	}

	return res;
}

