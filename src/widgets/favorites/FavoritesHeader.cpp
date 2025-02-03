//
// Created by TheDaChicken on 7/24/2024.
//

#include "FavoritesHeader.h"
#include "utils/Log.h"

#include "models/ChannelModel.h"
#include "delegates/ChannelDelegate.h"

#include <QPainter>

FavoritesHeader::FavoritesHeader(FavoritesList *favorites_list, QWidget *parent)
	: LineHeader(parent), favorites_list_(favorites_list)
{
	assert(favorites_list_ != nullptr);

	leftText_ = tr("Favorites");
}

void FavoritesHeader::paintEvent(QPaintEvent *event)
{
	const QVariant editable = favorites_list_->model()->headerData(
		0,
		Qt::Horizontal,
		ChannelModel::kIsMoveable);

	if (editable.isValid() && editable.toBool())
	{
		rightText_ = tr("Edit");
	}
	else
	{
		rightText_.clear();
	}

	LineHeader::paintEvent(event);
}
