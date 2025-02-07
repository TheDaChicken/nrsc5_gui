//
// Created by TheDaChicken on 10/8/2024.
//

#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include <QListView>
#include <QStackedWidget>

#include <settings/TunerPage.h>
#include <settings/AboutPage.h>

#include "widgets/dualview/DualViewContainer.h"

class SettingsView : public DualViewContainer
{
	public:
		SettingsView();

	private:
		QStackedWidget *stacked_widget_;
		QListView *settings_panel_;

		AboutPage *about_page_;
		TunerPage *tuner_page_;
	private slots:
		void SwitchPage(const QModelIndex &index) const;
};

#endif //SETTINGSVIEW_H
