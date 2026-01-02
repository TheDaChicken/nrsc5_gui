//
// Created by TheDaChicken on 11/11/2025.
//

#include "HybridMetadata.h"

#include "utils/Log.h"
//
// void HybridMetadata::ProcessHybridEvent(const HybridEvent &event)
// {
// 	switch (event.type)
// 	{
// 		case STATION_ID:
// 		{
// 			OnStationId(*std::static_pointer_cast<NRSC5::IdFrame>(event.obj));
// 			break;
// 		}
// 		case STATION_NAME:
// 		{
// 			OnStationName(*std::static_pointer_cast<NRSC5::NameFrame>(event.obj));
// 			break;
// 		}
// 		case STATION_ID3:
// 		{
// 			OnStationID3(*std::static_pointer_cast<NRSC5::ID3>(event.obj));
// 			break;
// 		}
// 		case STATION_LOT:
// 		{
// 			OnStationLot(*std::static_pointer_cast<NRSC5::Lot>(event.obj));
// 			break;
// 		}
// 		case STATION_PROGRAM_SERVICE:
// 		{
// 			OnStationProgram(*std::static_pointer_cast<NRSC5::ProgramFrame>(event.obj));
// 			break;
// 		}
// 		default:
// 			break;
// 	}
// }

void HybridMetadata::OnStationProgram(const NRSC5::ProgramFrame frame)
{
	auto &program = data.programs[frame.id];

	program.type = frame.type;
	program.id = frame.id;
}

void HybridMetadata::OnStationID3(const NRSC5::ID3 &id3)
{
	auto &program = data.programs[id3.program_id];
	const unsigned int friendlyId = NRSC5::FriendlyProgramId(id3.program_id);

	if (!id3.artist.empty() || !id3.title.empty())
		Logger::Log(info,
		            "HD{}: Artist={} Title={}",
		            friendlyId,
		            id3.artist,
		            id3.title);

	if (!id3.album.empty())
		Logger::Log(info,
		            "HD{}: Album={}",
		            friendlyId,
		            id3.album);

	if (!id3.genre.empty())
		Logger::Log(info,
		            "HD{}: Genre={}",
		            friendlyId,
		            id3.genre);

	if (!id3.xhdr.Empty())
		Logger::Log(info,
		            "HD{}: XHDR: {} Lot={} mime={}",
		            friendlyId,
		            id3.xhdr.ParamName(),
		            id3.xhdr.lot,
		            NRSC5::DescribeMime(id3.xhdr.mime));

	program.id3_ = id3;
}

void HybridMetadata::OnStationId(const NRSC5::IdFrame &frame)
{
	Logger::Log(info,
	            "HDRadio: Station ID: {} ({})",
	            frame.id,
	            frame.country_code);

	data.idFrame_ = frame;
}

void HybridMetadata::OnStationName(const NRSC5::NameFrame &frame)
{
	data.nameFrame_ = frame;
}

void HybridMetadata::OnStationLot(NRSC5::Lot &lot)
{
}
