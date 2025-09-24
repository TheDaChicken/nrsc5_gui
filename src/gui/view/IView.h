//
// Created by TheDaChicken on 9/20/2025.
//

#ifndef IVIEW_H
#define IVIEW_H

#include "gui/managers/ThemeManager.h"

class IView
{
	public:
		virtual ~IView()
		{
		}

		virtual void Render(const Theme &theme) = 0;
};

#endif //IVIEW_H
