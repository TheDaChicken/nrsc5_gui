//
// Created by TheDaChicken on 7/13/2025.
//

#include "ButtonGroup.h"

void GUI::ButtonGroup::Render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(40, 40));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(40.0f, 40.0f));

	const float child_width = buttonSize_.x + ImGui::GetStyle().WindowPadding.x * 2; // 80px for the button width + padding

	ImGui::BeginChild("LeftButtons", ImVec2(child_width, 0), ImGuiChildFlags_AlwaysUseWindowPadding);
	{
		ImGui::BeginGroup();

		for (auto &button : buttons_)
		{
			button.Render(buttonSize_);
		}

		ImGui::EndGroup();
	}
	ImGui::EndChild();

	ImGui::PopStyleVar(2);
}
