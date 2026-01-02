//
// Created by TheDaChicken on 9/18/2025.
//

#include "RadioBackView.h"

#include "gui/widgets/Navigation.h"

bool RadioBackView::RenderNavigation(const RenderContext &theme)
{
	if (Navigation::RenderHeader(
		theme.theme,
		IconType::Back,
		"Back"))
		return true;

	return false;
}

bool RadioBackView::RenderCenter(const RenderContext &theme)
{
	return tune_panel_.Render(theme.theme);
}
