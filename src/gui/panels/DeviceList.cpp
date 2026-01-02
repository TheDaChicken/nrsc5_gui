//
// Created by TheDaChicken on 12/18/2025.
//

#include "DeviceList.h"

#include <imgui.h>

#include "utils/Log.h"

void DeviceList::Render(const InputDevicesState &state)
{
	std::string_view selected_name;

	if (!state.selected_device.has_value())
		selected_name = "No Selected Device";
	else
		selected_name = state.selected_device->name;

	if (ImGui::BeginCombo("SDRDevices", selected_name.data()))
	{
		if (state.devices.empty())
			ImGui::Text("No SDR devices found");

		for (auto &device : state.devices)
		{
			const std::string &name = device.name;
			//bool selected = dev;

			ImGui::PushID(&device);

			if (ImGui::Selectable(name.c_str(), false))
			{
				if (!state.OpenDevice)
					Logger::Log(err, "OpenDevice() not defined");
				else
					state.OpenDevice(device);
			}

			ImGui::PopID();
		}

		ImGui::EndCombo();
	}
}
