//
// Created by TheDaChicken on 11/11/2025.
//

#include "FavoritesController.h"

FavoritesController::FavoritesController(FavoriteList &favorite_model)
	: m_selectedItem(0), favorite_model_(favorite_model)
{
	favorite_model_.on_update = [this]
	{
		m_items.resize(favorite_model_.GetChannelCount());

		for (int i = 0; i < favorite_model_.GetChannelCount(); ++i)
		{
			const Station &fav_station = favorite_model_.GetChannel(i);
			FavoriteItem &item = m_items[i];

			item.station = &fav_station;
			item.name = item.station->GetShort();

			// if (external_)
			// {
			// 	const MetadataQuery query{
			// 		item.station->identity.country_code,
			// 		item.station->identity.name,
			// 		item.station->program_id,
			// 		MimeQuery::StationLogo,
			// 	};
			//
			// 	item.icon = external_->FetchImageAsync(query);
			// }
		}

		// Keep selection valid
		if (m_selectedItem >= favorite_model_.GetChannelCount())
			m_selectedItem = 0;
	};
}
