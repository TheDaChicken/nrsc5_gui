//
// Created by TheDaChicken on 8/7/2025.
//

#include "DockAudioPanel.h"

#include "gui/managers/ThemeManager.h"
#include "audio/AudioManager.h"

DockAudioPanel::DockAudioPanel()
{
}

void DockAudioPanel::Render(const Theme &theme)
{
	ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_medium_size);

	ImGui::BeginGroup();
	RenderAudioCombo();
	ImGui::EndGroup();

	ImGui::PopFont();
}

void DockAudioPanel::RenderAudioCombo()
{
	auto &audio_manager = AUDIO::AudioManager::GetInstance();
	const char *selected_name = audio_manager.GetCurrentDeviceName();

	if (!ImGui::BeginCombo("AudioDevices", selected_name))
		return;

	const std::vector<AUDIO::Device> &devices = AUDIO::AudioManager::GetPlaybackDevices();

	if (devices.empty())
		ImGui::Text("No Devices found");

	for (int i = 0; i < devices.size(); i++)
	{
		auto &device = devices[i];

		ImGui::PushID(i);

		if (ImGui::Selectable(device.name.data(), selected_name == device.name.data()))
		{
			audio_manager.OpenAudioDevice(&device);
		}

		ImGui::PopID();
	}

	ImGui::EndCombo();
}

