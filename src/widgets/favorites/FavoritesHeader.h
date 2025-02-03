//
// Created by TheDaChicken on 7/24/2024.
//

#ifndef NRSC5_GUI_SRC_CHANNELS_FAVORITESHEADER_H_
#define NRSC5_GUI_SRC_CHANNELS_FAVORITESHEADER_H_

#include "FavoritesList.h"
#include "widgets/LineHeader.h"

class FavoritesHeader : public LineHeader
{
		Q_OBJECT

	public:
		explicit FavoritesHeader(FavoritesList *favorites_list, QWidget *parent = nullptr);

	private:
		void paintEvent(QPaintEvent *event) override;
		FavoritesList *favorites_list_;
};

#endif //NRSC5_GUI_SRC_CHANNELS_FAVORITESHEADER_H_
