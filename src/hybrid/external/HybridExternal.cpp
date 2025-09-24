//
// Created by TheDaChicken on 7/22/2025.
//

#include "HybridExternal.h"

#include "gui/image_decoders/ImageDecoder.h"
#include "nrsc5/LotManager.h"

HybridExternal::HybridExternal(
	const std::shared_ptr<ImageManager> &image_manager,
	DatabaseManager &db_manager)
	: image_manager_(image_manager), lot_service_(db_manager)
{
	assert(image_manager_);
}

TextureHandle HybridExternal::FetchImageAsync(const MetadataQuery &query)
{
	const std::string key = GenerateMetadataKey(query);

	auto [added, handle] = image_manager_->CreateImageCache(key);

	if (added)
	{
		ThreadPool::GetInstance().QueueJob([this, query, key]
		{
			if (auto image_ref = lot_service_.FetchImage(query))
				image_manager_->QueueImage(key, image_ref.value());
		});
	}
	return handle;
}

void HybridExternal::ReceivedLotImage(
	const StationIdentity &station,
	const NRSC5::Lot &lot)
{
	MetadataQuery query{
		station.country_code,
		station.name,
	};

	query.program_id = lot.component.programId.value();

	if (lot.component.mime == NRSC5_MIME_STATION_LOGO)
	{
		query.type = MimeQuery::StationLogo;
	}
	else if (lot.component.mime == NRSC5_MIME_PRIMARY_IMAGE)
	{
		query.type = MimeQuery::PrimaryLogo;
		query.xhr_id = static_cast<int>(lot.id);
	}
	else
	{
		// We only care about station logos and primary logos for now
		return;
	}

	const std::string key = GenerateMetadataKey(query);
	if (!image_manager_->ContainsImage(key))
		return;

	GUI::ImageBuffer imageLot;

	const auto ret = lot_service_.LoadLotImage(lot, imageLot);
	if (ret == GUI::FileLoadResult::Success)
	{
		image_manager_->QueueImage(key, imageLot);

		// If we received an image for the current lot, update the primary image.
		Logger::Log(debug,
		            "HybridExternal: Updated {} for subchannel {}",
		            NRSC5::DescribeMime(lot.component.mime),
		            query.program_id);
	}
}

std::string HybridExternal::GenerateMetadataKey(const MetadataQuery &query)
{
	return fmt::format("{}{}{}{}_{}",
	                   query.country_code,
	                   query.name,
	                   query.program_id,
	                   static_cast<int>(query.type),
	                   query.xhr_id);
}
