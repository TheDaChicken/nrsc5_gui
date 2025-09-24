//
// Created by TheDaChicken on 8/7/2025.
//

#ifndef DOCKAUDIOPANEL_H
#define DOCKAUDIOPANEL_H

#include "gui/managers/ThemeManager.h"

class DockAudioPanel
{
	public:
		explicit DockAudioPanel();

		void Render(const Theme &theme);

	private:
		void RenderAudioCombo();
};

#endif //DOCKAUDIOPANEL_H
