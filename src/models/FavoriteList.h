//
// Created by TheDaChicken on 7/22/2025.
//

#ifndef FAVORITEMODEL_H
#define FAVORITEMODEL_H

//#include "external/HybridExternal.h"
#include "RadioChannel.h"
#include "sql/DatabaseManager.h"

class FavoriteList
{
	public:
		std::function<void()> on_update;

		explicit FavoriteList(
			DatabaseManager &db_manager);

		[[nodiscard]] const Station &GetChannel(const int index) const
		{
			if (index < 0 || index >= static_cast<int>(m_favorites.size()))
				throw std::out_of_range("Favorite channel index out of range");

			return m_favorites[index];
		}

		[[nodiscard]] int GetChannelCount() const
		{
			return static_cast<int>(m_favorites.size());
		}

		bool Update();
		bool Submit();

	private:
		DatabaseManager &db_manager_;
		std::vector<Station> m_favorites;
};

#endif //FAVORITEMODEL_H
