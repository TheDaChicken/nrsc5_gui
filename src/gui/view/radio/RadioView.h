//
// Created by TheDaChicken on 8/3/2025.
//

#ifndef RADIOVIEW_H
#define RADIOVIEW_H

#include "RadioBackView.h"
#include "RadioHomeView.h"
#include "gui/panels/DockFavoritesPanel.h"
#include "gui/managers/ThemeManager.h"
#include "gui/view/IView.h"

enum Page
{
	Home = 0,
	Settings = 1,
};

class RadioView final : public IView
{
	public:
		explicit RadioView(
			const std::shared_ptr<UISession> &session
		)
			: radio_home_(session),
			  radio_back_(session),
			  session_(session)
		{
		}

		void Render(RenderContext &context) override;
		void RenderNavigation(const RenderContext &theme);
		void RenderCenter(const Theme &theme);

	private:
		void RenderRadio(RenderContext &theme);

		Page current_page_ = Home;
		RadioHomeView radio_home_;
		RadioBackView radio_back_;
		const std::shared_ptr<UISession> &session_;
};

#endif //RADIOVIEW_H
