//
// Created by TheDaChicken on 7/22/2025.
//

#ifndef STATIONINFOMANAGER_H
#define STATIONINFOMANAGER_H

#include "services/LotService.h"
#include "hybrid/HybridState.h"
#include "gui/managers/ImageManager.h"
#include "sql/DatabaseManager.h"
#include "nrsc5/LotManager.h"

class HybridExternal
{
	public:
		explicit HybridExternal(const std::shared_ptr<ImageManager> &image_manager,
		                        DatabaseManager &db_manager);

		TextureHandle FetchImageAsync(const MetadataQuery &query);

		void ReceivedLotImage(const StationIdentity &station,
		                      const NRSC5::Lot &lot);

	private:
		std::string GenerateMetadataKey(const MetadataQuery &query);

		std::shared_ptr<ImageManager> image_manager_;
		LotService lot_service_;
};

#endif //STATIONINFOMANAGER_H
