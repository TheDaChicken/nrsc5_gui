//
// Created by TheDaChicken on 8/3/2025.
//

#include "SettingsView.h"

#include "../managers/ThemeManager.h"
#include "gui/widgets/ListView.h"
#include "gui/widgets/Navigation.h"

void SettingsView::Render(RenderContext &context)
{
	if (Navigation::BeginNavigation(context.theme, "SettingsNav"))
	{
		Navigation::BeginNavList();
		RenderSettingList(context.theme);

		Navigation::BeginCenter();
		RenderCenter(context);

		Navigation::EndNav();
	}
}

void SettingsView::RenderSettingList(const Theme &theme)
{
	Navigation::RenderHeader(theme, IconType::UNKNOWN, "Settings");

	ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_medium_size);

	ImGui::BeginChild("Settings", ImVec2(0, 0));
	ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0);

	for (int i = 0; i < views_.size(); ++i)
	{
		ListView::Item item;
		item.name = views_[i].first;

		if (ListView::RenderItem(item, i == selected_setting_))
			selected_setting_ = i;
	}

	ImGui::PopStyleVar();

	ImGui::EndChild();
	ImGui::PopFont();
}

void SettingsView::RenderCenter(RenderContext &theme)
{
	if (views_.empty())
		return;

	auto&[name, view] = views_[selected_setting_];

	assert(view);

	view->Render(theme);
}
