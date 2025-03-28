//
// Created by TheDaChicken on 9/16/2024.
//

#include "ImageText.h"

#include <QPainter>
#include <QSize>

#include "utils/Image.h"

ImageText::ImageText(QWidget *parent)
	: QFrame(parent),
	  alignment_(TextRight | ImageCenterX),
	  spacing_(20),
	  text_width_(0),
	  extra_spacing_(5)
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setContentsMargins(0, 0, 0, 0);
}

void ImageText::setPixmap(const QPixmap &pixmap)
{
	if (image_.cacheKey() != pixmap.cacheKey())
	{
		image_ = pixmap;
		scaled_pixmap_ = QPixmap();
		update();
	}
}

void ImageText::setImage(const QImage &image)
{
	setPixmap(QPixmap::fromImage(
		UTILS::Image::CreateBestOptionalBackground(image,
		                                           palette().color(backgroundRole()),
		                                           palette().color(QPalette::AlternateBase))));
}

void ImageText::paintEvent(QPaintEvent *)
{
	const QRect layoutRect = contentsRect();
	QPainter painter(this);
	drawFrame(&painter);

	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::TextAntialiasing);

	EnsureTextDocument();
	// Widget Scaling is usually done in the paint event. Okay to scale pixmap here
	EnsureScaledPixmap();

	// To align with text, we need to move the pixmap upwards. Use scaled size
	DrawItems(painter, layoutRect, scaled_pixmap_);
}

void ImageText::EnsureScaledPixmap()
{
	if (image_.isNull())
		return;

	const QSize kWidgetSize = contentsRect().size();
	const QSize maximumSize = MaximumImageSize(kWidgetSize);
	const qreal pixelRatio = devicePixelRatioF();
	const QSize bestSize = image_.size().scaled(maximumSize * pixelRatio,
	                                            Qt::KeepAspectRatio);

	if (scaled_pixmap_.isNull() || scaled_pixmap_.size() != bestSize)
	{
		QPixmap resize = image_.scaled(
			bestSize,
			Qt::KeepAspectRatio,
			Qt::SmoothTransformation);
		resize.setDevicePixelRatio(pixelRatio);

		scaled_pixmap_ = resize;
	}
}

void ImageText::DrawItems(QPainter &painter, const QRect &layoutRect, const QPixmap &pixmap) const
{
	const QSizeF pixmapSize(pixmap.deviceIndependentSize());
	const QSizeF kMinimumSize(MinimumWidgetSize(pixmapSize));
	QRectF widgetRect(layoutRect);

	AlignWidgetRect(widgetRect, kMinimumSize);

	const QRectF pixmapRect = CalculatePixmapRect(widgetRect, kMinimumSize, pixmapSize);
	const QRectF textRect = CalculateTextRect(widgetRect, kMinimumSize, pixmapRect);

	// Paint the image & move the text rect
	painter.drawPixmap(pixmapRect.topLeft(), pixmap);

	// Paint the text
	painter.save();
	painter.translate(textRect.topLeft());
	text_document_.drawContents(&painter);
	painter.restore();
}

void ImageText::AlignWidgetRect(QRectF &widgetRect, const QSizeF &minimumSize) const
{
	if (alignment_ & CenterX && minimumSize.width() != 0)
		widgetRect.adjust((widgetRect.width() - minimumSize.width()) / 2, 0, 0, 0);
	if (alignment_ & CenterY && minimumSize.height() != 0)
		widgetRect.adjust(0, (widgetRect.height() - minimumSize.height()) / 2, 0, 0);
}

QRectF ImageText::CalculatePixmapRect(const QRectF &widgetRect, const QSizeF &minimumSize,
                                      const QSizeF &pixmapSize) const
{
	QRectF pixmapPos = widgetRect.toRect();

	if (alignment_ & ImageCenterX || (alignment_ & CenterX && minimumSize.width() == 0))
		pixmapPos.setX(widgetRect.x() + (widgetRect.width() - pixmapSize.width()) / 2);
	if (alignment_ & ImageCenterY || (alignment_ & CenterY && minimumSize.height() == 0))
		pixmapPos.setY(widgetRect.y() + (widgetRect.height() - pixmapSize.height()) / 2);

	pixmapPos.setSize(pixmapSize);
	return pixmapPos;
}

QRectF ImageText::CalculateTextRect(const QRectF &widgetRect,
                                    const QSizeF &minimumSize,
                                    const QRectF &pixmapRect) const
{
	const QSizeF kTextSize(TextSize());
	QRectF textRect = widgetRect;

	if (alignment_ & CenterX && minimumSize.width() == 0)
		textRect.setX(widgetRect.x() + static_cast<float>(widgetRect.width() - kTextSize.width()) / 2);
	if (alignment_ & CenterY && minimumSize.height() == 0)
		textRect.setY(widgetRect.y() + static_cast<float>(widgetRect.height() - kTextSize.height()) / 2);

	if (alignment_ & TextLeft)
		textRect.setWidth(textRect.width() - pixmapRect.width() - spacing_);
	else if (alignment_ & TextRight)
		textRect.setX(pixmapRect.x() + pixmapRect.width() + spacing_);
	else if (alignment_ & TextTop)
		textRect.setHeight(textRect.height() - (-pixmapRect.height() + spacing_));
	else if (alignment_ & TextBottom)
		textRect.setY(pixmapRect.y() + pixmapRect.height() + spacing_);

	return textRect;
}

QSizeF ImageText::MinimumWidgetSize(const QSizeF &pixmap) const
{
	EnsureTextDocument();

	const QSizeF textSize = TextSize();
	QSizeF qSize(0, 0);

	// We need space for the text + spacing + image depending on the alignment
	if (alignment_ & TextLeft || alignment_ & TextRight)
	{
		qSize.setWidth(pixmap.width() + spacing_ + textSize.width());
		qSize.setHeight(0); // No width constraint
	}
	else if (alignment_ & TextTop || alignment_ & TextBottom)
	{
		qSize.setWidth(0); // No width constraint
		qSize.setHeight(pixmap.height() + spacing_ + textSize.height());
	}

	return qSize;
}

QSizeF ImageText::TextSize() const
{
	EnsureTextDocument();
	return text_document_.size();
}

/**
 * Calculates the maximum rect of the image.
 * @param size The size of the widget.
 * @return
 */
QSize ImageText::MaximumImageSize(const QSize &size) const
{
	const QSizeF textSize = TextSize();
	QSize maxPixmapSize = size;

	if (alignment_ & TextLeft || alignment_ & TextRight)
		maxPixmapSize.setWidth(qCeil(size.width() - textSize.width() - spacing_));
	else if (alignment_ & TextTop || alignment_ & TextBottom)
		maxPixmapSize.setHeight(qCeil(size.height() - textSize.height() - spacing_));

	return maxPixmapSize;
}

QSize ImageText::sizeHint() const
{
	EnsureTextDocument();

	const QSizeF textSize = TextSize();
	const QSizeF pixmap = image_.deviceIndependentSize();
	QSize q_size;

	// We need to require space for the text + spacing + image

	if (alignment_ & TextLeft || alignment_ & TextRight)
	{
		q_size.setWidth(qCeil(pixmap.width() + textSize.width() + spacing_));
		q_size.setHeight(qCeil(qMax(pixmap.height(), textSize.height())));
	}
	else if (alignment_ & TextTop || alignment_ & TextBottom)
	{
		q_size.setWidth(qCeil(qMax(pixmap.width(), textSize.width())));
		q_size.setHeight(qCeil(pixmap.height() + textSize.height() + spacing_));
	}
	return q_size;
}

int ImageText::heightForWidth(const int width) const
{
	const QSizeF textSize = TextSize();
	int height_max = pixmap().isNull()
		                 ? 0
		                 : static_cast<int>(static_cast<qreal>(pixmap().height()) * width / pixmap().width());

	if (alignment_ & TextLeft || alignment_ & TextRight)
	{
		height_max = qCeil(qMax(static_cast<qreal>(height_max), textSize.height()));
	}
	else if (alignment_ & TextTop || alignment_ & TextBottom)
	{
		height_max += qCeil(textSize.height() + spacing_);
	}
	return height_max;
}
