//
// Created by TheDaChicken on 9/26/2023.
//

#include "models/ChannelModel.h"
#include "ChannelDelegate.h"
#include "utils/Log.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QPaintDevice>

#define DELETE_BUTTON ":/buttons/RedMinus.svg"

void ChannelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	static QPixmap deleteIcon(DELETE_BUTTON);

	Q_ASSERT(index.isValid());

	const QRect contentRect(option.rect.adjusted(
			kChannelIconMargin,
			kChannelMargin,
			-kChannelIconMargin,
			-kChannelMargin)
	);
	const auto freq = index.data(Qt::DisplayRole).toString();
	const auto icon = index.data(Qt::DecorationRole).value<QPixmap>();
	const auto deletable = index.data(ChannelModel::kIsDeletable).toBool();

	QFont font(option.font);
	font.setHintingPreference(QFont::HintingPreference::PreferNoHinting);

	painter->save();
	painter->setClipRect(option.rect);
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setFont(font);

	if (option.state & QStyle::State_Selected)
	{
		QPainterPath path;
		path.addRoundedRect(option.rect, 10, 10);

		painter->fillPath(path, option.palette.highlight());
	}

	// Center the icon vertically
	QSize iconSize = kChannelIconSize;

	if (style_ == kBoxLayout)
	{
		// Use the width of the content rect for the icon size in box layout
		iconSize = {contentRect.width(), kChannelIconSize.height()};
	}

	const QPixmap iconScaled = CacheScaleIcon(icon, iconSize, painter->device()->devicePixelRatioF());
	const QRectF iconRect = CalculateIconRect(contentRect, iconScaled.deviceIndependentSize());
	const QRect textRect = CalculateTextRect(contentRect, iconRect);

	painter->save();
	painter->drawPixmap(iconRect.topLeft(), iconScaled);

	// Draw the frequency text
	painter->setPen(QPen(option.palette.text(), 1));

	if (style_ == kBoxLayout)
	{
		painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter, freq);
	}
	else
	{
		painter->drawText(textRect, Qt::AlignVCenter, freq);
	}

	painter->restore();

	// Delete button is not displayed in the box layout
	if (style_ == kDefault && deletable)
	{
		const QPixmap deleteScaled = CacheScaleIcon(deleteIcon,
		                                            kChannelDeleteSize,
		                                            painter->device()->devicePixelRatioF());
		const QRect deleteRect = CalculateDeleteButtonRect(contentRect);

		// Draw the delete button vertically centered
		painter->drawPixmap(deleteRect, deleteScaled);
	}

	painter->restore();
}

QSize ChannelDelegate::sizeHint(const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
	if (style_ == kBoxLayout)
	{
		const QFontMetrics fontMetrics(option.font);

		// Use the longest text to determine the width of each item
		const QSize textSize = fontMetrics.size(0, "106.1 HD4");

		// Height: Icon size + text size + margins
		return {
			textSize.width(),
			kChannelIconSize.height() + textSize.height() + kChannelBoxMargin * 2
		};
	}
	if (style_ == kDefault)
	{
		// Height: Icon size + margins
		return {kChannelIconSize.width() + kChannelDeleteSize.width(), kChannelIconSize.height() + kChannelMargin * 2};
	}
	return {};
}

bool ChannelDelegate::editorEvent(QEvent *event,
                                  QAbstractItemModel *model,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index)
{
	bool ret = false;

	if (event->type() == QEvent::MouseButtonPress)
	{
		const QRect deleteRect = CalculateDeleteButtonRect(option.rect);

		auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);

		if (deleteRect.isValid() && deleteRect.contains(mouseEvent->pos()))
			ret = model->removeRow(index.row(), index);

		if (ret)
			event->accept();
	}

	return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QRectF ChannelDelegate::CalculateIconRect(const QRect &contentRect, const QSizeF iconSize) const
{
	if (style_ == kBoxLayout)
	{
		// Icon is displayed at the top of the content rect
		// Center the icon horizontally & vertically
		const float xIconOffset = (contentRect.width() - iconSize.width()) / 2;
		return {
			contentRect.x() + xIconOffset, static_cast<qreal>(contentRect.y()),
			iconSize.width(), iconSize.height()
		};
	}

	// Icon is displayed at the left of the content rect
	// Center the icon vertically
	const int yIconOffset = (contentRect.height() - iconSize.height()) / 2;
	return {QPoint(contentRect.x(), contentRect.y() + yIconOffset), iconSize};
}

QRect ChannelDelegate::CalculateTextRect(const QRect &contentRect, const QRectF &iconRect) const
{
	if (style_ == kBoxLayout)
	{
		// Text is displayed below the icon
		return {
			QPoint(contentRect.x(), iconRect.bottom() + kChannelMargin),
			contentRect.bottomRight()
		};
	}
	if (style_ == kDefault)
	{
		// Text is displayed to the right of the icon
		return contentRect.adjusted(iconRect.width() + kChannelMargin, 0, 0, 0);
	}
	return {};
}

QRect ChannelDelegate::CalculateDeleteButtonRect(const QRect &contentRect) const
{
	if (style_ == kBoxLayout)
		return {};

	// Button is display at the right of the content rect and centered vertically
	const int xDeleteOffset = contentRect.width() - kChannelDeleteSize.width();
	const int yDeleteOffset = (contentRect.height() - kChannelDeleteSize.height()) / 2;

	return {
		contentRect.x() + xDeleteOffset,
		contentRect.y() + yDeleteOffset,
		kChannelDeleteSize.width(), kChannelDeleteSize.height()
	};
}

// QItemDelegate's hacky but faster version of serializing quint64 to a QString
static QString qPixmapSerial(quint64 i)
{
	ushort arr[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	ushort *ptr = &arr[16];

	while (i > 0)
	{
		// hey - it's our internal representation, so use the ascii character after '9'
		// instead of 'a' for hex
		*(--ptr) = '0' + i % 16;
		i >>= 4;
	}

	return QString(reinterpret_cast<const QChar *>(ptr),
	               static_cast<int>(&arr[sizeof(arr) / sizeof(ushort)] - ptr));
}

QPixmap ChannelDelegate::CacheScaleIcon(const QPixmap &icon, const QSize &size, qreal deviceRatio)
{
	const QString key = qPixmapSerial(icon.cacheKey());

	QPixmap scaled;
	QSize scaledSize = size * deviceRatio;

	if (!QPixmapCache::find(key, &scaled) || scaled.size() != scaledSize)
	{
		scaled = icon.scaled(scaledSize,
		                     Qt::KeepAspectRatio,
		                     Qt::SmoothTransformation);
		scaled.setDevicePixelRatio(deviceRatio);

		QPixmapCache::insert(key, scaled);
	}

	return scaled;
}
