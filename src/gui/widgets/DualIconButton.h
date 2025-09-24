//
// Created by TheDaChicken on 7/25/2025.
//

#ifndef DUALIMAGEBUTTON_H
#define DUALIMAGEBUTTON_H

#include <string>

#include "IconButton.h"

namespace GUI
{
struct DualIconButton
{
	enum class Result { None, Left, Right };

	std::string button_id;
	std::string label;
	IconButton left;
	IconButton right;

	Result Render() const;
};
}

#endif //DUALIMAGEBUTTON_H
