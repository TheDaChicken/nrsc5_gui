//
// Created by TheDaChicken on 8/7/2025.
//

#include "DockAudioPanel.h"

#include "gui/managers/ThemeManager.h"
#include "../platform/sdl/SDLAudioManager.h"

DockAudioPanel::DockAudioPanel()
{
}

void DockAudioPanel::Render(RenderContext &context)
{
	ImGui::PushFont(
		context.theme.GetFont(FontType::Semibold),
		context.theme.font_medium_size);

	ImGui::PopFont();
}
