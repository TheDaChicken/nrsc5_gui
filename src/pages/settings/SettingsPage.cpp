//
// Created by TheDaChicken on 9/26/2024.
//

#include "SettingsPage.h"

#include "views/SettingsView.h"

SettingsPage::SettingsPage(QWidget *parent): DualViewWidget(parent)
{
	AddWidget(new SettingsView());
}
