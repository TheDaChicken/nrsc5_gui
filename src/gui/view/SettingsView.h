//
// Created by TheDaChicken on 8/3/2025.
//

#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include "IView.h"
#include "gui/panels/DockInputPanel.h"
#include "gui/managers/ThemeManager.h"
#include "gui/panels/DockAudioPanel.h"

class SettingsView : public IView
{
	public:
		explicit SettingsView(
			DockInputPanel &input_panel,
			DockAudioPanel &audio_panel)
			: input_panel(input_panel),
			  audio_panel(audio_panel)
		{
		}

		void Render(const Theme &theme) override;

		void RenderSettingList(const Theme &theme);
		void RenderCenter(const Theme &theme);
	private:
		DockInputPanel &input_panel;
		DockAudioPanel &audio_panel;
		int selected_setting_ = 0;
};

#endif //SETTINGSVIEW_H
