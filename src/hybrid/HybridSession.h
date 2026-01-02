//
// Created by TheDaChicken on 12/12/2025.
//

#ifndef NRSC5_GUI_HYBRIDSESSION_H
#define NRSC5_GUI_HYBRIDSESSION_H

#include <memory>
#include "input/RadioInput.h"

class HybridSession
{
	public:
		HybridSession();

		// Disable copy
		HybridSession(const HybridSession&) = delete;
		HybridSession& operator=(const HybridSession&) = delete;

		void PushInput(std::shared_ptr<IRadioInput>&& input);
		InputStatus SetFrequency(Band::Type type, uint32_t freq);

		InputStatus Start() const;
		InputStatus Stop() const;

		IRadioInput::ISDRControl* GetSDRControl()
		{
			if (!input_)
				return {};
			return input_->GetISDRControl();
		}

	private:
		std::shared_ptr<IRadioInput> input_;
		HybridDecoder decoder;
};

#endif //NRSC5_GUI_HYBRIDSESSION_H