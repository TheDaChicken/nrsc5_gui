//
// Created by TheDaChicken on 10/6/2025.
//

#ifndef NRSC5_GUI_SDRSETTINGS_H
#define NRSC5_GUI_SDRSETTINGS_H

#include "gui/view/IView.h"
#include "gui/UISession.h"

struct InputDevice
{
	std::vector<PortSDR::GainMode> gain_modes;
	std::vector<PortSDR::Gain> gain_stages;

	PortSDR::GainMode gain_mode;
	std::map<std::string, int> gain_value;

	std::function<void(std::string_view, int gains)> SetGain;
	std::function<void(const PortSDR::GainMode&)> SetGainMode;
};

class SDRSettings
{
	public:
		explicit SDRSettings();

		//void Render(RenderContext &context);
	private:
		void RenderSDRGainModes(const InputDevice &context);
		void RenderGains(const InputDevice &context);
};

#endif //NRSC5_GUI_SDRSETTINGS_H
