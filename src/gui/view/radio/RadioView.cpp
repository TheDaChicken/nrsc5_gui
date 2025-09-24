//
// Created by TheDaChicken on 8/3/2025.
//

#include "RadioView.h"

#include "gui/widgets/Navigation.h"

void RadioView::Render(const Theme &theme)
{
	if (input_.GetStatus() == Playing)
	{
		if (Navigation::BeginNavigation(theme, "SettingsNav"))
		{
			Navigation::BeginNavList();
			RenderNavigation(theme);
			Navigation::BeginCenter();
			RenderCenter(theme);

			Navigation::EndNav();
		}
	}
	else
	{
		RenderCenter(theme);
	}
}

void RadioView::RenderNavigation(const Theme &theme)
{
	if (current_page_ == Home)
	{
		if (radio_home_.RenderNavigation(theme))
			current_page_ = Settings;
	}
	else
	{
		if (radio_back_.RenderNavigation(theme))
			current_page_ = Home;
	}
}

void RadioView::RenderCenter(const Theme &theme)
{
	switch (input_.GetStatus())
	{
		case Playing:
		{
			RenderRadio(theme);
			break;
		}
		case Idle:
		{
			ImGui::Text("No Tuner Connected");
			break;
		}
		case Stopping:
		{
			break;
		}
		case Starting:
		{
			break;
		}
		case Error:
		{
			ImGui::Text("Error");
			break;
		}
		default:
			break;
	}
}

void RadioView::RenderRadio(const Theme &theme)
{
	if (current_page_ == Home)
		radio_home_.RenderCenter(theme);
	else if (radio_back_.RenderCenter(theme))
		current_page_ = Home;
}
