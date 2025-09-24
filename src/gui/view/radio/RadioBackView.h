//
// Created by TheDaChicken on 9/18/2025.
//

#ifndef RADIOBACKVIEW_H
#define RADIOBACKVIEW_H

#include "gui/managers/ThemeManager.h"
#include "gui/panels/DockTunePanel.h"
#include "hybrid/HybridInput.h"
#include "hybrid/HybridSession.h"
#include "models/FavoriteList.h"

class RadioBackView
{
	public:
		RadioBackView(const std::shared_ptr<HybridExternal> &external,
		              FavoriteList &favorites,
		              HybridInput &input,
		              const std::shared_ptr<HybridSession> &stream)
			: tune_panel_(input),
			  input_(input)
		{
		}

		bool RenderNavigation(const Theme &theme);
		bool RenderCenter(const Theme &theme);

	private:
		DockTunePanel tune_panel_;
		HybridInput &input_;
};

#endif //RADIOBACKVIEW_H
