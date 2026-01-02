//
// Created by TheDaChicken on 9/9/2025.
//

#ifndef HYBRIDLOT_H
#define HYBRIDLOT_H

#include "RadioChannel.h"
#include "images/ImageBuffer.h"
#include "sql/DatabaseManager.h"
#include "sql/LotTable.h"

enum ExternalServiceRet
{
	ExternalService_Success,
	ExternalService_Failure,
	ExternalService_NotFound,
};

class LotService
{
	public:
		explicit LotService(DatabaseManager &db_manager)
			: db_manager_(db_manager)
		{
		}

		tl::expected<GUI::ImageBuffer, ExternalServiceRet> FetchImage(
			const MetadataQuery &query);

		static GUI::FileLoadResult LoadLotRecord(const LotRecord &lot, GUI::ImageBuffer &image);
		static GUI::FileLoadResult LoadLotImage(const NRSC5::Lot &lot, GUI::ImageBuffer &image);
		static GUI::FileType LotType(uint32_t type);

	private:
		DatabaseManager &db_manager_;
};

#endif //HYBRIDLOT_H
