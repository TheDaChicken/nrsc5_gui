//
// Created by TheDaChicken on 11/9/2025.
//

#ifndef NRSC5_GUI_STATIONCONTROLLER_H
#define NRSC5_GUI_STATIONCONTROLLER_H

#include "Events.h"
#include "RadioChannel.h"

class StationController
{
	public:
		// explicit StationController(
		// 	HybridState &state);

		void ProgramChanged(unsigned int program);
		void ProcessLot(const StationIdentity &identity, const NRSC5::Lot &lot);

	private:
		//void OnFrequencyChange(const FrequencyChangeFrame &change_frame);
		void OnStationName(const NRSC5::NameFrame &change_frame);
		void OnStationId(const NRSC5::IdFrame &change_frame);
		void OnStationID3(const NRSC5::ID3 &id3);
		void StationUpdate();
		void FetchStationLogo();
		void FetchPrimaryImage(int xhdr);

		//HybridState &state_;
};

#endif //NRSC5_GUI_STATIONCONTROLLER_H
