//
// Created by TheDaChicken on 7/23/2025.
//

#include "Navigation.h"

#include "gui/managers/ThemeManager.h"

bool Navigation::BeginNavigation(const Theme &theme, const std::string &id)
{
	//ImGui::PushStyleVarY(ImGuiStyleVar_CellPadding, 0);
	const bool success = ImGui::BeginTable(
		id.c_str(),
		2,
		ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp |
		ImGuiTableFlags_NoSavedSettings);

	if (success)
	{
		ImGui::TableSetupColumn("Nav", ImGuiTableColumnFlags_WidthStretch, 2.0f);
		ImGui::TableSetupColumn("Center", ImGuiTableColumnFlags_WidthStretch, 5.0f);
	}
	return success;
}

void Navigation::BeginNavList()
{
	ImGui::TableNextColumn();
	ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
	                       ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg)));
}

void Navigation::BeginCenter()
{
	ImGui::TableNextColumn();
	ImGui::BeginChild("Center",
	                  ImVec2(0, 0));
}

void Navigation::EndNav()
{
	ImGui::EndChild();
	ImGui::EndTable();
}

bool Navigation::RenderHeader(
	const Theme &theme,
	const IconType &icon_type,
	const std::string_view name,
	const std::string_view sub_text)
{
	ImGui::Spacing();
	ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_large_size);

	bool clicked = false;
	const std::shared_ptr<GUI::SVGImage> icon = theme.GetIcon(icon_type);

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().FramePadding.x);

	// Place icon related to name. Can be used to go back
	ImGui::BeginGroup();
	{
		const float icon_space = ImGui::GetFontSize() * 1.5f;

		if (icon)
		{
			// Reserve icon space with a dummy
			ImGui::Dummy(ImVec2(icon_space, 0));
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			// Draw the icon inside that space, vertically centered
			const ImVec2 start_pos = {
				ImGui::GetItemRectMin().x + (icon_space - icon->GetInlineSize().x) * 0.5f,
				ImGui::GetItemRectMin().y
			};
			icon->DrawInline(start_pos);
		}
		else
		{
			ImGui::Spacing();
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		}

		ImGui::TextUnformatted(name.data(), name.data() + name.size());
	}
	ImGui::EndGroup();
	if (ImGui::IsItemClicked())
		clicked = true;

	// Current sub menu
	if (!sub_text.empty())
	{
		ImGui::SameLine(0.0, ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::TextUnformatted(sub_text.data(), sub_text.data() + sub_text.size());
	}

	ImGui::PopFont();
	return clicked;
}
