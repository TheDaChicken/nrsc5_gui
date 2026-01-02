//
// Created by TheDaChicken on 8/3/2025.
//

#ifndef FAVORITESPANEL_H
#define FAVORITESPANEL_H

#include "AppState.h"
#include "gui/managers/ThemeManager.h"

class DockFavoritesPanel
{
	public:
		~DockFavoritesPanel() = default;

		bool Render(const RenderContext &context);

	private:
		void RenderHeader(const Theme &theme);
		bool RenderItems(const RenderContext &context) const;
};

#endif //FAVORITESPANEL_H
