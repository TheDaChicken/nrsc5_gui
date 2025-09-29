//
// Created by TheDaChicken on 8/4/2025.
//

#ifndef DOCKINPUTPANEL_H
#define DOCKINPUTPANEL_H

#include "gui/managers/ThemeManager.h"
#include "hybrid/HybridInput.h"
#include "hybrid/HybridSessionManager.h"

struct TunerState
{
	std::vector<PortSDR::Gain> gainStages;

	std::vector<std::string> gainModes;
	std::string currentGainMode;

	PortSDR::Gain currentGain;
	std::map<std::string, float> currentGains;
};

class DockInputPanel
{
	public:
		explicit DockInputPanel(HybridInput &input);

		void Render(const Theme &theme);

	private:
		void RenderSDRDevices();
		void RenderSDRGainModes();
		void RenderGains();

		void RenderSDRGain(const PortSDR::Gain &gain);

		HybridInput &input_;

		std::vector<std::shared_ptr<PortSDR::Device>> available_devices_;
		std::shared_ptr<PortSDR::Device> selected_device_;
		PortSDR::PortSDR sdr;
		bool freely_gain = true;
};

#endif //DOCKINPUTPANEL_H
