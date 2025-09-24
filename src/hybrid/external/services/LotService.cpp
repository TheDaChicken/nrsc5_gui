//
// Created by TheDaChicken on 9/9/2025.
//

#include "LotService.h"

#include "gui/image_decoders/ImageDecoder.h"

tl::expected<GUI::ImageBuffer, ExternalServiceRet> LotService::FetchImage(
	const MetadataQuery &query)
{
	auto db_ref = db_manager_.PopConnection();
	if (!db_ref)
	{
		Logger::Log(err,
		            "LotService: Failed to get database connection {}",
		            static_cast<int>(db_ref.error()));
		return tl::unexpected(ExternalService_Failure);
	}

	LotTable table(db_ref.value());
	LotRecord lot;
	lot.callsign = query.name;
	lot.channel = NRSC5::FriendlyProgramId(query.program_id);
	lot.service = query.type == MimeQuery::StationLogo ? NRSC5_MIME_STATION_LOGO : NRSC5_MIME_PRIMARY_IMAGE;
	lot.id = query.xhr_id;

	tl::expected<LotRecord, SQLiteError> value_ref;

	if (query.type == MimeQuery::StationLogo)
		value_ref = table.GetLotSpecial(lot);
	else
		value_ref = table.GetLot(lot);

	if (!value_ref)
	{
		if (value_ref.error() == Lite_Done)
			return tl::unexpected(ExternalService_NotFound);
		return tl::unexpected(ExternalService_Failure);
	}

	GUI::ImageBuffer image;
	const int ret = LoadLotRecord(value_ref.value(), image);
	if (ret != GUI::FileLoadResult::Success)
	{
		Logger::Log(err,
		            "HybridExternalService: Failed to load {} lot for station {}-HD{}: {}",
		            NRSC5::DescribeMime(lot.service),
		            query.name,
		            NRSC5::FriendlyProgramId(query.program_id),
		            static_cast<int>(ret));
		return tl::unexpected(ExternalService_Failure);
	}
	return image;
}

GUI::FileLoadResult LotService::LoadLotRecord(
	const LotRecord &lot,
	GUI::ImageBuffer &image)
{
	// If it's already loaded, use it
	const GUI::FileLoadResult ret = GUI::ImageDecoder::LoadImageFromFile(
		lot.path,
		image,
		LotType(lot.mime));

	if (ret == GUI::FileLoadResult::Success)
	{
		image.uri = "lot:" + lot.path.string();
	}

	return ret;
}

GUI::FileLoadResult LotService::LoadLotImage(
	const NRSC5::Lot &lot,
	GUI::ImageBuffer &image)
{
	// If it's already loaded, use it
	const GUI::FileLoadResult ret = GUI::ImageDecoder::LoadImageFromData(
		lot.data,
		image,
		LotType(lot.mime));

	if (ret == GUI::FileLoadResult::Success)
	{
		image.uri = "lot:" + lot.name;
	}

	return ret;
}

GUI::FileType LotService::LotType(const uint32_t type)
{
	switch (type)
	{
		case NRSC5_MIME_PNG:
			return GUI::FileType::PNG;
		case NRSC5_MIME_JPEG:
			return GUI::FileType::JPEG;
		default:
			return GUI::FileType::Unknown;
	}
}
