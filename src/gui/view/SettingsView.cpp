//
// Created by TheDaChicken on 8/3/2025.
//

#include "SettingsView.h"

#include "../managers/ThemeManager.h"
#include "gui/widgets/ListView.h"
#include "gui/widgets/Navigation.h"

void SettingsView::Render(const Theme &theme)
{
	if (Navigation::BeginNavigation(theme, "SettingsNav"))
	{
		Navigation::BeginNavList();
		RenderSettingList(theme);

		Navigation::BeginCenter();
		RenderCenter(theme);

		Navigation::EndNav();
	}
}

void SettingsView::RenderSettingList(const Theme &theme)
{
	Navigation::RenderHeader(theme, IconType::UNKNOWN, "Settings");

	const ListView::Item items[] = {
		{"Tuner"},
		{"Audio"},
		{"Network"},
		{"About"},
	};

	ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_medium_size);

	ImGui::BeginChild("Settings", ImVec2(0, 0));
	ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0);

	for (int i = 0; i < 4; ++i)
	{
		if (ListView::RenderItem(items[i], i == selected_setting_))
			selected_setting_ = i;
	}

	ImGui::PopStyleVar();

	ImGui::EndChild();
	ImGui::PopFont();
}

void SettingsView::RenderCenter(const Theme &theme)
{
	switch (selected_setting_)
	{
		case 0:
		{
			input_panel.Render(theme);
			break;
		}
		case 1:
		{
			audio_panel.Render(theme);
			break;
		}
		default:
			break;
	}
}
