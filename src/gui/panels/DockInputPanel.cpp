//
// Created by TheDaChicken on 8/4/2025.
//

#include "DockInputPanel.h"

#include <Device.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <memory>
#include <vector>

#include "gui/managers/ThemeManager.h"

DockInputPanel::DockInputPanel(HybridInput &input)
	: input_(input)
{
	selected_device_ = std::make_shared<PortSDR::Device>();
	selected_device_->name = "No Device Selected";
	selected_device_->index = -1;

	available_devices_ = sdr.GetDevices();
}

void DockInputPanel::Render(const Theme &theme)
{
	ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_small_size);

	ImGui::BeginGroup();
	if (ImGui::BeginCombo("SDRDevices", selected_device_->name.c_str()))
	{
		RenderSDRDevices();
		ImGui::EndCombo();
	}
	RenderSDRGainModes();
	RenderGains();
	ImGui::EndGroup();

	ImGui::PopFont();
}

void DockInputPanel::RenderSDRDevices()
{
	static float timer = 0.0f;
	timer += ImGui::GetIO().DeltaTime;

	if (timer >= 1.0f) // refresh every 1.0 seconds
	{
		available_devices_ = sdr.GetDevices();
		timer = 0.0f;
	}

	if (selected_device_->index != -1)
		ImGui::Selectable(selected_device_->name.c_str(), true);
	else if (available_devices_.empty())
		ImGui::Text("No SDR devices found");

	for (int i = 0; i < available_devices_.size(); i++)
	{
		const auto &device = available_devices_[i];
		const std::string name = device->name;

		ImGui::PushID(i);

		if (ImGui::Selectable(name.c_str(), false))
		{
			if (input_.OpenSDR(device))
			{
				selected_device_ = device;
				available_devices_.erase(available_devices_.begin() + i);
				i++;
			}
		}

		ImGui::PopID();
	}
}

void DockInputPanel::RenderSDRGainModes()
{
	if (input_.GetInput() == nullptr)
		return;

	const auto modes = input_.GetInput()->GetGainModes();

	if (modes.empty())
		return;

	if (ImGui::RadioButton("Free", freely_gain))
		freely_gain = !freely_gain;

	for (const auto &mode : modes)
	{
		const auto curr_mode = input_.GetInput()->GetGainMode();

		if (ImGui::RadioButton(mode.c_str(), mode == curr_mode))
		{
			input_.GetInput()->SetGainMode(mode);

			freely_gain = false;
		}
	}
}

void DockInputPanel::RenderGains()
{
	if (input_.GetInput() == nullptr)
		return;

	if (freely_gain)
	{
		for (const PortSDR::Gain &gain : input_.GetInput()->GetGainStages())
			RenderSDRGain(gain);
	}
	else
		RenderSDRGain(input_.GetInput()->GetGainStage());
}

bool SliderFloatWithSteps(const char *label, float *v, float v_min, float v_max, float v_step,
                          const char *display_format = "&.3f")
{
	char text_buf[64] = {};
	ImFormatString(text_buf, IM_ARRAYSIZE(text_buf), display_format, *v);

	// Map from [v_min,v_max] to [0,N]
	const int countValues = int((v_max - v_min) / v_step);
	int v_i = int((*v - v_min) / v_step);
	const bool value_changed = ImGui::SliderInt(label, &v_i, 0, countValues, text_buf);

	// Remap from [0,N] to [v_min,v_max]
	*v = v_min + float(v_i) * v_step;
	return value_changed;
}

void DockInputPanel::RenderSDRGain(const PortSDR::Gain &gain)
{
	const auto step = static_cast<float>(gain.range.step());
	float value = static_cast<float>(input_.GetInput()->GetGain(gain.stage));

	if (SliderFloatWithSteps(gain.stage.c_str(),
	                         &value,
	                         static_cast<float>(gain.range.min()),
	                         static_cast<float>(gain.range.max()),
	                         step,
	                         "%.1f"))
	{
		input_.GetInput()->SetGain(gain.stage, value);
	}
}
