//
// Created by TheDaChicken on 8/3/2025.
//

#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include "IView.h"
#include "gui/panels/DockInputPanel.h"
#include "gui/managers/ThemeManager.h"

class SettingsView final : public IView
{
	public:
		explicit SettingsView()
		{
		}

		void AddView(const std::string &name, const std::shared_ptr<IView> &view)
		{
			views_.emplace_back(name, view);
		}

		void Render(RenderContext &context) override;

		void RenderSettingList(const Theme &theme);
		void RenderCenter(RenderContext &theme);

	private:
		int selected_setting_ = 0;

		std::vector<std::pair<std::string, std::shared_ptr<IView> > > views_;
};

#endif //SETTINGSVIEW_H
