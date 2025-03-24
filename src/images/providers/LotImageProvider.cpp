//
// Created by TheDaChicken on 8/23/2024.
//

#include "LotImageProvider.h"
#include "utils/StringUtils.h"

LotImageProvider::LotImageProvider(LotManager *lot_manager)
	: lot_manager_(lot_manager)
{
}

ImageData LotImageProvider::FetchStationImage(const Channel &channel)
{
	// Get HD Station Image if available

	NRSC5::Lot lot;
	if (lot_manager_->GetStationImage(channel.station_info, lot))
	{
		return LoadLotImage(lot);
	}

	return {};
}

ImageData LotImageProvider::FetchPrimaryImage(
	const Channel &channel,
	const NRSC5::ID3 &id3)
{
	if (id3.xhdr.lot == -1)
	{
		return {};
	}

	// if a hint is provided, try to use it as an option
	NRSC5::Lot lot;
	lot.id = id3.xhdr.lot;
	lot.component.channel = NRSC5::FriendlyProgramId(channel.station_info.current_program);
	lot.component.mime = id3.xhdr.mime;

	if (lot_manager_->GetLot(channel.station_info, lot))
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
		const QString path = UTILS::PathToQStr(lot.path);

		ret = pixmap.load(path, format);
		uri = "file:" + path;
	}

	if (!ret)
		return {};

	return {pixmap, uri, ImageData::Type::kLotImage};
}
