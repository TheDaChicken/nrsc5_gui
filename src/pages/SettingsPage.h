//
// Created by TheDaChicken on 9/26/2024.
//

#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <views/SettingsView.h>
#include <widgets/dualview/DualViewWidget.h>

class SettingsPage : public DualViewWidget
{
		Q_OBJECT

	public:
		explicit SettingsPage(QWidget *parent = nullptr);

		SettingsView* GetSettingsView() const;
	private:
		SettingsView* settings_view_;
};

#endif //SETTINGSPAGE_H
