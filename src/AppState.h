//
// Created by TheDaChicken on 11/28/2025.
//

#ifndef NRSC5_GUI_APPSTATE_H
#define NRSC5_GUI_APPSTATE_H

#include <memory>

#include "gui/platform/sdl/SDLAudioManager.h"
#include "gui/controllers/FavoritesController.h"
#include "gui/controllers/SDRController.h"
#include "gui/managers/ThemeManager.h"
#include "gui/panels/DeviceList.h"

struct SessionUi
{
	DeviceStatus status;

	std::optional<DeviceUi> device;

	bool running;
	uint32_t frequency;

	std::vector<PortSDR::GainMode> gain_modes;
	std::vector<PortSDR::Gain> gain_stages;
	std::map<std::string, double> gains_values;

	PortSDR::GainMode gain_mode;
};

struct AppContext
{
	std::shared_ptr<SDRController> rc;
	std::shared_ptr<AUDIO::SDLAudioManager> am;
	std::shared_ptr<FavoritesController> fc;
};

struct RenderContext
{
	Theme theme;

	int selected_host;
	bool freely_gain = false;

	SessionUi session;
	std::shared_ptr<AppContext> app;
};

#endif //NRSC5_GUI_APPSTATE_H
