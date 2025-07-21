//
// Created by TheDaChicken on 7/13/2025.
//

#include "Imgui.h"

#include <cstdio>
#include <imgui.h>

ScopedImgui::ScopedImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
}

ScopedImgui::~ScopedImgui()
{
	ImGui::DestroyContext();
}
