//
// Created by TheDaChicken on 7/13/2025.
//

#ifndef TOUCHSCREENBUTTONS_H
#define TOUCHSCREENBUTTONS_H
#include <algorithm>
#include <functional>
#include <string>

#include "ImageButton.h"

namespace GUI
{
struct ButtonGroup
{
	std::vector<CircleImageButton> buttons_;
	ImVec2 buttonSize_ = ImVec2(75, 75);

	void Render();
};
}

#endif //TOUCHSCREENBUTTONS_H
