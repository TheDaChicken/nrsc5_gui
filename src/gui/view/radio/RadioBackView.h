//
// Created by TheDaChicken on 9/18/2025.
//

#ifndef RADIOBACKVIEW_H
#define RADIOBACKVIEW_H

#include "AppState.h"
#include "gui/managers/ThemeManager.h"
#include "gui/panels/DockTunePanel.h"
#include "gui/UISession.h"
#include "models/FavoriteList.h"

class RadioBackView
{
	public:
		explicit RadioBackView(const std::shared_ptr<UISession>& input)
			: tune_panel_(input)
		{
		}

		bool RenderNavigation(const RenderContext &theme);
		bool RenderCenter(const RenderContext &theme);

	private:
		DockTunePanel tune_panel_;
};

#endif //RADIOBACKVIEW_H
