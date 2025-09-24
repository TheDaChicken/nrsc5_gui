//
// Created by TheDaChicken on 7/23/2025.
//

#ifndef NAVIGATIONHEADER_H
#define NAVIGATIONHEADER_H

#include <string_view>

#include "gui/managers/ThemeManager.h"

class Navigation
{
	public:
		static bool BeginNavigation(const Theme &theme, const std::string &id);
		static void BeginNavList();
		static void BeginCenter();
		static void EndNav();
		static bool RenderHeader(const Theme &theme,
		                         const IconType &icon_type,
		                         std::string_view name,
		                         std::string_view sub_text = "");
};

#endif //NAVIGATIONHEADER_H
