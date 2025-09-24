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
		RadioHomeView(const std::shared_ptr<HybridExternal> &external,
		              FavoriteList &favorites,
		              HybridInput &input,
		              const std::shared_ptr<HybridSession> &stream)
			: favorites_panel_(external, favorites),
			  station_panel_(input, stream),
			  input_(input)
		{
		}

		bool RenderNavigation(const Theme &theme);
		void RenderCenter(const Theme &theme);

	private:
		DockFavoritesPanel favorites_panel_;
		DockStationPanel station_panel_;

		HybridInput &input_;
};

#endif //RADIOHOMEVIEW_H
