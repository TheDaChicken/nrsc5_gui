//
// Created by TheDaChicken on 8/7/2025.
//

#ifndef DOCKAUDIOPANEL_H
#define DOCKAUDIOPANEL_H

#include "AppState.h"
#include "gui/view/IView.h"

class DockAudioPanel final : public IView
{
	public:
		explicit DockAudioPanel();

		void Render(RenderContext &context) override;
};

#endif //DOCKAUDIOPANEL_H
