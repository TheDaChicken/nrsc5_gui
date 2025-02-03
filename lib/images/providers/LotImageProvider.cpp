//
// Created by TheDaChicken on 8/23/2024.
//

#include "LotImageProvider.h"
#include "utils/StringUtils.h"

LotImageProvider::LotImageProvider(LotManager *lot_manager)
	: lot_manager_(lot_manager)
{
}

ImageData LotImageProvider::FetchStationImage(const RadioChannel &channel)
{
	// Get HD Station Image if available

	if (NRSC5::Lot lot; lot_manager_->SQLGetStationImage(channel.hd_station_, lot))
	{
		return LoadLotImage(lot);
	}

	return {QPixmap(), "", ImageData::Type::kMissing};
}

ImageData LotImageProvider::FetchPrimaryImage(const RadioChannel &channel,
                                              const NRSC5::ID3 &id3)
{
	if (id3.xhdr.lot == -1)
	{
		return {};
	}

	// if a hint is provided, try to use it as an option
	NRSC5::Lot lot;
	lot.id = id3.xhdr.lot;

	if (lot_manager_->GetLot(channel.hd_station_, lot))
	{
		return LoadLotImage(lot);
	}

	return {};
}

ImageData LotImageProvider::LoadLotImage(const NRSC5::Lot &lot)
{
	QPixmap pixmap;
	QString uri;
	const char *format = std::string(NRSC5::DescribeMime(lot.mime)).c_str();
	bool ret = false;

	if (!lot.data.empty())
	{
		// If it's already loaded, use it
		ret = pixmap.loadFromData(lot.data.data(),
		                          lot.data.size(),
		                          format);
		uri = "data:" + QString::fromStdString(lot.name);
	}
	else
	{
		// Otherwise, load it from the path
		ret = pixmap.load(StringUtils::PathToStr(lot.path), format);
		uri = "file:" + StringUtils::PathToStr(lot.path);
	}

	if (!ret)
		return {QPixmap(), "", ImageData::Type::kMissing};

	return {pixmap, uri, ImageData::Type::kLotImage};
}
