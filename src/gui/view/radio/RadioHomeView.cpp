//
// Created by TheDaChicken on 9/18/2025.
//

#include "RadioHomeView.h"

#include "gui/widgets/Navigation.h"

bool RadioHomeView::RenderNavigation(const Theme &theme)
{
	if (Navigation::RenderHeader(
		theme,
		IconType::Back,
		"Radio",
		input_.Sessions().GetState().frequency_text))
		return true;

	if (favorites_panel_.Render(theme))
	{
		const auto &station = favorites_panel_.GetSelectedStation();
		input_.SetChannel(station);
	}
	return false;
}

void RadioHomeView::RenderCenter(const Theme &theme)
{
	station_panel_.Render(theme);
}
