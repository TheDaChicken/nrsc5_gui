//
// Created by TheDaChicken on 8/22/2024.
//

#include "StationInfoManager.h"
#include "Application.h"
#include "utils/Log.h"
#include "images/providers/LotImageProvider.h"

#include <QtConcurrentRun>

StationInfoManager::StationInfoManager(SQLite::Database &database)
	: lot_manager_(database)
{
	station_logo_ = image_provider_.MissingImage();
	primary_image_ = image_provider_.MissingImage();
	favorites_model_ = std::make_unique<FavoriteModel>(database, image_provider_, this);

	image_provider_.AddProvider(
		std::make_shared<LotImageProvider>(&lot_manager_),
		1);
}

/**
 * @brief Style and display the station information.
 * This is run every single time the station information is updated.
 * @param channel The radio channel.
 */
void StationInfoManager::StyleAndDisplayStation(
	const ActiveChannel &channel)
{
	const NRSC5::StationInfo &stationInfo = channel.station_info;

	// If the station is not similar, update the station information.
	if (station_info_ != stationInfo)
	{
		station_info_ = stationInfo;
		station_logo_ = image_provider_.FetchStationImage(channel);
		ClearID3();
	}

	emit UpdateActiveChannel(channel);
	emit UpdateStationLogo(station_logo_.image);

	DisplayFavorite(channel);
}

/**
* @brief If we have received a new Station Logo, update the station logo cache.
* @param station The station for which the lot was received.
* @param lot The lot that was received.
*/
void StationInfoManager::ReceiveLot(
	const NRSC5::StationInfo &station,
	const NRSC5::Lot &lot)
{
	if (lot.component.programId.has_value()
		&& lot.component.programId.value() == station_info_.current_program)
	{
		if (lot.component.mime == NRSC5_MIME_PRIMARY_IMAGE
			&& station_id3_.xhdr.lot == lot.id)
		{
			// If we received an image for the current lot, update the primary image.
			Logger::Log(debug,
			            "StationInfoManager: Received Primary Image LOT ID={}",
			            lot.id);

			const ImageData imageLot = LotImageProvider::LoadLotImage(lot);
			if (imageLot.IsEmpty())
				return;

			DisplayPrimaryImage(imageLot);
		}
		else if (lot.component.mime == NRSC5_MIME_STATION_LOGO)
		{
			// We received a new station logo. Update the station logo.
			Logger::Log(debug,
			            "StationInfoManager: Received Station Logo LOT ID={}",
			            lot.id);

			const ImageData stationLogo = LotImageProvider::LoadLotImage(lot);
			if (stationLogo.IsEmpty())
				return;

			station_logo_ = stationLogo;
			emit UpdateStationLogo(station_logo_.image);

			// If the primary image is labeled "missing", update the primary image to the new station logo.
			if (primary_image_.IsMissing())
			{
				DisplayFallbackPrimaryImage();
			}
		}
	}

	// Save the lot to the database
	QFuture<void> future = QtConcurrent::run([this, station, lot]
	{
		lot_manager_.LotReceived(station, lot);
	});
}

/**
 * @brief Display the favorite status of the channel.
 * @param channel The radio channel.
 */
void StationInfoManager::DisplayFavorite(
	const Channel &channel)
{
	const QModelIndex index = favorites_model_->Find(channel);
	if (index.isValid() && !station_info_.name.empty())
	{
		// Update the favorite if it exists with the new channel
		if (const Channel &favorite = favorites_model_->Get(index.row());
			favorite.station_info.name != station_info_.name)
		{
			// If the station name is different, update the favorite.
			// This is to prevent the favorite from being out of sync.
			favorites_model_->Set(index.row(), channel);
		}
	}

	emit UpdateFavorite(index);
}

void StationInfoManager::DisplayFallbackPrimaryImage()
{
	DisplayPrimaryImage(FallbackPrimaryImage());
}

void StationInfoManager::DisplayPrimaryImage(
	const ImageData &image)
{
	emit UpdatePrimaryImage(image.image);
	primary_image_ = image;
}

/**
 * @brief Style and display the ID3 information.
 * This is run every single time the ID3 information is updated.
 * @param id3 The ID3 information to display.
 */
void StationInfoManager::StyleAndDisplayID3(
	const NRSC5::ID3 &id3)
{
	if (station_id3_ != id3)
	{
		station_id3_ = id3;
		emit UpdateID3(id3);
		FetchPrimaryImage(id3);
	}
}

void StationInfoManager::ClearID3()
{
	station_id3_.Clear();
	emit ClearId3();
	DisplayFallbackPrimaryImage();
}

/**
 * @brief Fetch the supplementary image for the ID3.
 * @param id3 The ID3 information.
 */
void StationInfoManager::FetchPrimaryImage(
	const NRSC5::ID3 &id3)
{
	const ActiveChannel &channel = getApp()->GetRadioController().GetChannel();

	// HDRadio alerted us that we display primary image
	// This is to prevent non-primary images from being displayed.
	// FIXME: Are there really different display types?
	// There is a problem where only Fetching Primary Image is only when HD says to show primary image.
	// What if we don't want to display that?? Add the ability to change that.
	if (id3.xhdr.mime == NRSC5_MIME_PRIMARY_IMAGE)
	{
		Logger::Log(debug,
		            "StationInfoManager: Fetching Primary Image LOT ID={}",
		            id3.xhdr.lot);

		// We don't want the current primary image anymore.
		// Cancel old primary image future if it's not finished.
		primary_image_future_ = FetchPrimaryImage(channel, id3);
		primary_image_future_.then([this](const QFuture<ImageData> &future)
		{
			const ImageData image = future.result();

			if (image.IsMissing())
			{
				// If the result is default, display the default primary image.
				Logger::Log(warn, "StationInfoManager: Fetched empty. Fallback to Default Primary Image.");

				DisplayPrimaryImage(FallbackPrimaryImage());
			}
			else
			{
				Logger::Log(debug,
				            "StationInfoManager: Fetching Primary Image Result: {}",
				            image.uri);

				// Display the primary image.
				DisplayPrimaryImage(image);
			}
		});
	}
}

QFuture<ImageData> StationInfoManager::FetchPrimaryImage(
	const Channel &channel,
	const NRSC5::ID3 &id3) const
{
	return QtConcurrent::run([this, channel, id3]
	{
		return image_provider_.FetchPrimaryImage(channel, id3);
	});
}

/**
* @brief Returns the fallback image for primary image.
* This will return Station logo if available.
* Otherwise, the fallback image for primary image.
*/
ImageData StationInfoManager::FallbackPrimaryImage() const
{
	ImageData missingImage = station_logo_.IsMissing()
		                         ? image_provider_.MissingImage()
		                         : station_logo_;

	// Identify this as a missing image that can be replaced by a primary image.
	missingImage.type = ImageData::kMissing;

	return missingImage;
}
