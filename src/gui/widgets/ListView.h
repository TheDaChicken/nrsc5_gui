//
// Created by TheDaChicken on 8/1/2025.
//

#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <memory>
#include <string>

#include "gui/managers/ThemeManager.h"

namespace ListView
{
struct Item
{
	std::string_view name;
	const GPU::Texture* image;
};

bool RenderItem(const Theme& theme, const Item &item, bool selected);
};

#endif //LISTVIEW_H
