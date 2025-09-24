//
// Created by TheDaChicken on 7/13/2025.
//

#ifndef BUTTON_H
#define BUTTON_H

#include <imgui.h>
#include <string>

#include "../utils/Icon.h"

namespace GUI
{
struct IconButton
{
	std::string id;
	std::shared_ptr<SVGImage> icon;
	ImColor checkedColor = IM_COL32(140, 181, 255, 50);

	bool Render(bool checked = false) const;
};
}

#endif //BUTTON_H
