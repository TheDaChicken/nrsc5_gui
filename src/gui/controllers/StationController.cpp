//
// Created by TheDaChicken on 11/9/2025.
//

#include "StationController.h"

void StationController::ProgramChanged(const unsigned int program)
{
	//state_.ui.program_id = program;
	StationUpdate();
}

void StationController::ProcessLot(
	const StationIdentity &identity,
	const NRSC5::Lot &lot)
{
	//external_->ReceivedLotImage(identity, lot);
}

/*void StationController::OnFrequencyChange(const FrequencyChangeFrame &change_frame)
{
	state_.ui = HybridStateUi();
	state_.ui.frequency_text = Band::FormatFrequency(
		change_frame.mode,
		change_frame.freq
	);
	StationUpdate();
}*/

void StationController::OnStationID3(const NRSC5::ID3 &id3)
{
	// if (id3.program_id != state_.ui.program_id)
	// 	return;
	//
	// if (state_.ui.id3_.xhdr != id3.xhdr)
	// 	FetchPrimaryImage(id3.xhdr.lot);
	//
	// state_.ui.id3_ = id3;
}

void StationController::StationUpdate()
{
	// if (state_.data.IsDry())
	// 	return;
	//
	// state_.ui.formatted_name = fmt::format(
	// 	"{}-HD{}",
	// 	state_.data.nameFrame_.name,
	// 	NRSC5::FriendlyProgramId(state_.ui.program_id));

	FetchStationLogo();
}

void StationController::FetchStationLogo()
{
	// const MetadataQuery query{
	// 	identity_.country_code,
	// 	identity_.name,
	// 	MimeQuery::StationLogo,
	// 	program_.id,
	// };
	//
	// state_.ui.station_logo_ = external_->FetchImageAsync(query);
}

void StationController::FetchPrimaryImage(int xhdr)
{
	// const MetadataQuery query{
	// 	identity_.country_code,
	// 	identity_.name,
	// 	MimeQuery::PrimaryLogo,
	// 	program_.id,
	// 	xhdr
	// };
	//
	// state_.ui.primary_logo_ = external_->FetchImageAsync(query);
}
