//
// Created by TheDaChicken on 8/23/2024.
//

#ifndef NRSC5_GUI_LIB_IMAGES_IMAGEDATA_H_
#define NRSC5_GUI_LIB_IMAGES_IMAGEDATA_H_

#include <QPixmap>

struct ImageData
{
	enum Type
	{
		kMissing,
		kLotImage // Image from LOTS
	};

	ImageData() : type(kMissing)
	{
	}

	ImageData(QPixmap image, QString uri, Type type)
		: image(std::move(image)), uri(std::move(uri)), type(type)
	{
	}

	QPixmap image;
	QString uri;
	Type type;

	[[nodiscard]] bool IsMissing() const
	{
		return type == kMissing;
	}

	[[nodiscard]] bool IsEmpty() const
	{
		return image.isNull();
	}
};

#endif //NRSC5_GUI_LIB_IMAGES_IMAGEDATA_H_
