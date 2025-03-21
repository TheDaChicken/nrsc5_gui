//
// Created by TheDaChicken on 3/21/2025.
//

#ifndef IMAGE_H
#define IMAGE_H

#include <qimage.h>

namespace UTILS
{
class Image
{
	public:
		static bool CheckBackgroundColor(const QImage &image, const QColor &background);
		static bool AlmostSimilarColors(const QColor &color1, const QColor &color2);
		static QImage CreateBestOptionalBackground(const QImage &image, const QColor &background,
		                                           const QColor &alternativeColor);
};
} // UTILS

#endif //IMAGE_H
