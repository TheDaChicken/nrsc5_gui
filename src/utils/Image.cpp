//
// Created by TheDaChicken on 3/21/2025.
//

#include "Image.h"

#include <QPainter>
#include <utils/Log.h>

namespace UTILS {

static constexpr int extraBackgroundMargins = 8;
static constexpr int colorThreshold = 2;

/**
* @brief Check if background color is optimal for the image.
* Image can be unable to be seen due to same color as the background.
*/
bool Image::CheckBackgroundColor(const QImage &image, const QColor &background)
{
	// If the image has no transparency, no change in background color is needed
	if (!image.hasAlphaChannel())
		return false;

	for (int y = 0; y < image.height(); y++)
	{
		const auto line = reinterpret_cast<const QRgb *>(image.constScanLine(y));
		for (int x = 0; x < image.width(); x++)
		{
			QColor color = QColor::fromRgba(line[x]);

			// ignore transparent pixels
			if (color.alpha() < 255)
				continue;

			if (!AlmostSimilarColors(color, background))
				return false;
		}
	}

	// Select secondary color for transparent images
	return true;
}

bool Image::AlmostSimilarColors(const QColor &color1, const QColor &color2)
{
	return qAbs(color1.red() - color2.red()) < colorThreshold &&
			qAbs(color1.green() - color2.green()) < colorThreshold &&
			qAbs(color1.blue() - color2.blue()) < colorThreshold;
}

QImage Image::CreateBestOptionalBackground(const QImage &image, const QColor& background, const QColor& alternativeColor)
{
	if (CheckBackgroundColor(image, background))
	{
		const QSize newSize = image.size() + QSize(extraBackgroundMargins, extraBackgroundMargins);

		QImage newImage(newSize, image.format());
		newImage.fill(alternativeColor);

		QPainter painter(&newImage);
		painter.drawImage(QPoint(extraBackgroundMargins / 2, extraBackgroundMargins / 2), image);

		return newImage;
	}
	return image;
}
} // UTILS