//
// Created by TheDaChicken on 10/6/2025.
//

#include "SDRSettings.h"

#include "gui/Util.h"

static std::string ConvertToStr(const PortSDR::GainMode mode)
{
	switch (mode)
	{
		case PortSDR::GAIN_MODE_FREE:
			return "Free";
		case PortSDR::GAIN_MODE_LINEARITY:
			return "Linear";
		case PortSDR::GAIN_MODE_SENSITIVITY:
			return "Sensitivity";
		default:
			return "Invalid";
	}
}

SDRSettings::SDRSettings()
{
}

// void SDRSettings::Render(RenderContext &context)
// {
// 	//SessionContext& session = context.session;
//
// 	//ImGui::BeginGroup();
//
// 	// if (session.session.GetSDRControl())
// 	// {
// 	// 	RenderSDRGainModes(context);
// 	// 	RenderGains(context);
// 	// }
//
// 	//ImGui::EndGroup();
// }

void SDRSettings::RenderSDRGainModes(const InputDevice &context)
{
	if (context.gain_modes.empty())
		return;

	for (const auto &mode : context.gain_modes)
	{
		const auto mode_str = ConvertToStr(mode);

		if (ImGui::RadioButton(mode_str.c_str(), mode == context.gain_mode))
			context.SetGainMode(mode);
	}
}

void SDRSettings::RenderGains(const InputDevice &context)
{
	for (const PortSDR::Gain &gain : context.gain_stages)
	{
		float value = 0;

		auto iter = context.gain_value.find(gain.stage);
		if (iter != context.gain_value.end())
			value = iter->second;

		const auto step = static_cast<float>(gain.range.Step());

		if (SliderFloatWithSteps(gain.stage.c_str(),
		                         &value,
		                         static_cast<float>(gain.range.Min()),
		                         static_cast<float>(gain.range.Max()),
		                         step,
		                         "%.1f"))
		{
			context.SetGain(gain.stage, value);
		}
	}
}



