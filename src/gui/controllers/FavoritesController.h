//
// Created by TheDaChicken on 11/11/2025.
//

#ifndef NRSC5_GUI_FAVORITESCONTROLLER_H
#define NRSC5_GUI_FAVORITESCONTROLLER_H

#include "RadioChannel.h"
#include "models/FavoriteList.h"

class FavoritesController
{
	public:
		explicit FavoritesController(FavoriteList &favorite_model);

		struct FavoriteItem
		{
			const Station *station;

			std::string name;
			GUI::TextureHandle icon;
		};

		const Station &GetSelectedStation() const
		{
			return favorite_model_.GetChannel(m_selectedItem);
		}

		int GetSelectedIndex() const
		{
			return m_selectedItem;
		}

		void SetSelectedIndex(const int index)
		{
			m_selectedItem = index;
		}

		const std::vector<FavoriteItem>& GetFavorites()
		{
			return m_items;
		}

	private:
		int m_selectedItem;
		FavoriteList &favorite_model_;
		std::vector<FavoriteItem> m_items;
		//const std::shared_ptr<HybridExternal> external_;
};

#endif //NRSC5_GUI_FAVORITESCONTROLLER_H
