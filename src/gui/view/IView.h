//
// Created by TheDaChicken on 9/20/2025.
//

#ifndef IVIEW_H
#define IVIEW_H

#include "AppState.h"

class IView
{
	public:
		virtual ~IView()
		{
		}

		virtual void Render(RenderContext &context) = 0;
};

#endif //IVIEW_H
