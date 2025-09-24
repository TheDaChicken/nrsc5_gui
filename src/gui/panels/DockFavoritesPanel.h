//
// Created by TheDaChicken on 8/3/2025.
//

#ifndef FAVORITESPANEL_H
#define FAVORITESPANEL_H

#include "models/FavoriteList.h"
#include "gui/managers/ThemeManager.h"
#include "hybrid/HybridInput.h"

class DockFavoritesPanel
{
	public:
		struct FavoriteItem
		{
			const Station *station;

			std::string name;
			TextureHandle icon;
		};

		explicit DockFavoritesPanel(const std::shared_ptr<HybridExternal> &external,
		                            FavoriteList &favorite_model);
		~DockFavoritesPanel() = default;

		bool Render(const Theme &theme);

		const Station &GetSelectedStation() const
		{
			return favorite_model_.GetChannel(m_selectedItem);
		}

	private:
		void RenderHeader(const Theme &theme);
		bool RenderItems(const Theme &theme);

		FavoriteList &favorite_model_;
		std::shared_ptr<HybridExternal> external_service_;
		std::vector<FavoriteItem> m_items;

		int m_selectedItem = 0;
};

#endif //FAVORITESPANEL_H
