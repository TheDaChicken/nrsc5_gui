//
// Created by TheDaChicken on 9/26/2024.
//

#include "SettingsPage.h"

SettingsPage::SettingsPage(QWidget *parent): DualViewWidget(parent)
{
	settings_view_ = new SettingsView();

	AddWidget(settings_view_);
}

SettingsView * SettingsPage::GetSettingsView() const
{
	return settings_view_;
}
