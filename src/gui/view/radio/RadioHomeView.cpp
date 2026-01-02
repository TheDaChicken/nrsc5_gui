//
// Created by TheDaChicken on 9/18/2025.
//

#include "RadioHomeView.h"

#include "gui/widgets/Navigation.h"

bool RadioHomeView::RenderNavigation(const RenderContext &context)
{
	if (Navigation::RenderHeader(
		context.theme,
		IconType::Back,
		"Radio",
		""))
		return true;

	if (favorites_panel_.Render(context))
	{
		const auto &station = context.app->fc->GetSelectedStation();
		//session_->SetChannel(station);
	}
	return false;
}

void RadioHomeView::RenderCenter(RenderContext &theme)
{
	station_panel_.Render(theme);
}
