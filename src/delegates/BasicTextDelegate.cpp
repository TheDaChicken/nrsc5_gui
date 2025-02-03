//
// Created by TheDaChicken on 9/9/2024.
//

#include "BasicTextDelegate.h"

#include <QPainter>
#include <QPainterPath>

#include "widgets/LineHeader.h"
#include "ChannelDelegate.h"

void BasicTextDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	constexpr int horizontalMargin = kTextMargin + ChannelDelegate::kChannelMargin;
	const QRect contentRect = option.rect.adjusted(
		horizontalMargin,
		ChannelDelegate::kChannelMargin,
		-horizontalMargin,
		-ChannelDelegate::kChannelMargin
	);

	const QVariant value = index.data(Qt::DisplayRole);

	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);

	if (option.state & QStyle::State_Selected)
	{
		QPainterPath path;
		path.addRoundedRect(option.rect, 10, 10);
		painter->fillPath(path, option.palette.highlight());
	}

	painter->drawText(contentRect, Qt::AlignLeft | Qt::AlignVCenter, value.toString());
	painter->restore();
}

QSize BasicTextDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return {
		ChannelDelegate::kChannelIconSize.width() + ChannelDelegate::kChannelMargin * 2,
		ChannelDelegate::kChannelIconSize.height() + ChannelDelegate::kChannelMargin * 2
	};
}
