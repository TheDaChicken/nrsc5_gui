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
			const std::shared_ptr<HybridExternal> &external,
			FavoriteList &favorites,
			HybridInput &input,
			const std::shared_ptr<HybridSession> &stream
		)
			: radio_home_(external, favorites, input, stream),
			  radio_back_(external, favorites, input, stream),
			  input_(input)
		{
		}

		[[nodiscard]] bool ShouldRenderNavigation() const
		{
			return input_.GetStatus() == Playing;
		}

		void Render(const Theme &theme);
		void RenderNavigation(const Theme &theme);
		void RenderCenter(const Theme &theme);

	private:
		void RenderRadio(const Theme &theme);

		Page current_page_ = Home;
		RadioHomeView radio_home_;
		RadioBackView radio_back_;
		HybridInput &input_;
};

#endif //RADIOVIEW_H
