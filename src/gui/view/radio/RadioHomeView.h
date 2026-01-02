//
// Created by TheDaChicken on 9/18/2025.
//

#ifndef RADIOHOMEVIEW_H
#define RADIOHOMEVIEW_H

#include "gui/managers/ThemeManager.h"
#include "gui/panels/DockFavoritesPanel.h"
#include "gui/panels/DockStationPanel.h"

class RadioHomeView
{
	public:
		explicit RadioHomeView(const std::shared_ptr<UISession> &stream)
			: station_panel_(stream)
		{
		}

		bool RenderNavigation(const RenderContext &theme);
		void RenderCenter(RenderContext &theme);

	private:
		DockFavoritesPanel favorites_panel_;
		DockStationPanel station_panel_;
};

#endif //RADIOHOMEVIEW_H
