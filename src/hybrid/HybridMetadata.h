//
// Created by TheDaChicken on 11/11/2025.
//

#ifndef NRSC5_GUI_HYBRIDMETADATA_H
#define NRSC5_GUI_HYBRIDMETADATA_H

#include "HybridDecoder.h"

struct HybridStateData
{
	NRSC5::NameFrame nameFrame_;
	NRSC5::IdFrame idFrame_;

	std::map<unsigned int, NRSC5::Program> programs;

	bool IsDry() const
	{
		return idFrame_.country_code.empty() || nameFrame_.name.empty();
	}

	void Reset()
	{
		*this = HybridStateData();
	}
};

class HybridMetadata
{
	public:
		// void ProcessEvent(const std::unique_ptr<HybridMessage> &message);
		// void ProcessFrequency(const HybridFrequencyFrame& frame);
		//void ProcessHybridEvent(const HybridEvent &event);

		HybridStateData &GetStateData()
		{
			return data;
		}

	private:
		void OnStationID3(const NRSC5::ID3 &id3);
		void OnStationId(const NRSC5::IdFrame &frame);
		void OnStationName(const NRSC5::NameFrame &frame);
		void OnStationProgram(NRSC5::ProgramFrame frame);
		void OnStationLot(NRSC5::Lot &lot);

		HybridStateData data;
};

#endif //NRSC5_GUI_HYBRIDMETADATA_H
