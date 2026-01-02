//
// Created by TheDaChicken on 12/18/2025.
//

#ifndef NRSC5_GUI_DEVICELIST_H
#define NRSC5_GUI_DEVICELIST_H

#include "input/RadioInput.h"

struct DeviceUi
{
	std::any id;
	std::string name;
};

struct InputDevicesState
{
	std::vector<DeviceUi> devices;
	std::optional<DeviceUi> selected_device;

	std::function<void(const DeviceUi&)> OpenDevice;
};

namespace DeviceList
{
	void Render(const InputDevicesState &state);
};

#endif //NRSC5_GUI_DEVICELIST_H