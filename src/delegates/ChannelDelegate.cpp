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

	const QRect contentRect(CalculateContentRect(option.rect));
	const QString text = index.data(Qt::DisplayRole).toString();
	const QPixmap icon = ReadIcon(index.data(Qt::DecorationRole));
	const bool deletable = index.data(ChannelModel::kIsDeletable).toBool();

	painter->save();
	painter->setClipRect(option.rect);
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setFont(option.font);

	if (option.state & QStyle::State_Selected)
	{
		QPainterPath path;
		path.addRoundedRect(option.rect, 10, 10);

		painter->fillPath(path, option.palette.highlight());
	}

	const QPixmap iconScaled = CacheScaleIcon(
		icon,
		MaxIconSize(contentRect),
		painter->device()->devicePixelRatioF()
	);
	const QRectF iconRect = CalculateIconRect(contentRect, iconScaled.deviceIndependentSize());
	const QRectF textRect = CalculateTextRect(contentRect, iconRect);

	painter->drawPixmap(iconRect, iconScaled, QRectF());

	painter->save();
	// Draw the frequency text
	painter->setPen(QPen(option.palette.text(), 1));

	if (style_ == kBoxLayout)
	{
		painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter, text);
	}
	else
	{
		painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
	}

	painter->restore();

	// Delete button is not displayed in the box layout
	if (style_ == kDefault && deletable)
	{
		const QPixmap deleteScaled = CacheScaleIcon(
			deleteIcon,
			kChannelDeleteSize,
			painter->device()->devicePixelRatioF()
		);
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

		// Height: Icon size + text size + top and bottom margins + margin between icon and text
		return {
			qMax(textSize.width(), kChannelBoxIconSize.width()) + kChannelIconMargin * 2,
			kChannelBoxIconSize.height() + textSize.height() + kChannelMargin * 2 + kChannelBoxTextMargin
		};
	}

	// Height: Icon size + margins
	return {
		kChannelIconSize.width() + kChannelDeleteSize.width(),
		kChannelIconSize.height() + kChannelMargin * 2
	};
}

bool ChannelDelegate::editorEvent(QEvent *event,
                                  QAbstractItemModel *model,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index)
{
	bool ret = false;

	if (event->type() == QEvent::MouseButtonPress)
	{
		const QRect contentRect = CalculateContentRect(option.rect);
		const QRect deleteRect = CalculateDeleteButtonRect(contentRect);

		auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);

		if (deleteRect.isValid() && deleteRect.contains(mouseEvent->pos()))
			ret = model->removeRow(index.row(), index);

		if (ret)
			event->accept();
	}

	return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QPixmap ChannelDelegate::ReadIcon(const QVariant &icon) const
{
	switch (icon.userType())
	{
		case QVariant::Icon:
			return icon.value<QIcon>().pixmap(kChannelIconSize);
		case QVariant::Pixmap:
			return icon.value<QPixmap>();
		default:
			return {};
	}
}

QSize ChannelDelegate::MaxIconSize(const QRect &contentRect) const
{
	QSize iconSize = kChannelIconSize;

	if (style_ == kBoxLayout)
	{
		// Use the width of the content rect for the icon size in box layout
		iconSize = {contentRect.width(), kChannelBoxIconSize.height()};
	}

	return iconSize;
}

QRect ChannelDelegate::CalculateContentRect(const QRect &rect) const
{
	return rect.adjusted(
		kChannelIconMargin,
		kChannelMargin,
		-kChannelIconMargin,
		-kChannelMargin);
}

QRectF ChannelDelegate::CalculateIconRect(const QRect &contentRect, const QSizeF iconSize) const
{
	QPointF iconPoint;

	if (style_ == kBoxLayout)
	{
		// Center the icon horizontally
		const qreal xIconOffset = (contentRect.width() - iconSize.width()) / 2;

		iconPoint = QPointF(contentRect.x() + xIconOffset, contentRect.y());
	}
	else
	{
		// Center the icon vertically
		const qreal yIconOffset = (contentRect.height() - iconSize.height()) / 2;

		iconPoint = QPointF(contentRect.x(), contentRect.y() + yIconOffset);
	}

	return {iconPoint, iconSize};
}

QRectF ChannelDelegate::CalculateTextRect(const QRect &contentRect, const QRectF &iconRect) const
{
	if (style_ == kBoxLayout)
	{
		// Text is displayed below the icon
		return {
			QPointF(contentRect.x(), iconRect.bottom() + kChannelBoxTextMargin),
			contentRect.bottomRight()
		};
	}

	// Text is displayed to the right of the icon
	return contentRect.toRectF().adjusted(iconRect.width() + kChannelMargin, 0, 0, 0);
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
