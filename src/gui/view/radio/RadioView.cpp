//
// Created by TheDaChicken on 8/3/2025.
//

#include "RadioView.h"

#include "gui/widgets/Navigation.h"

void RadioView::Render(RenderContext &context)
{
	const SessionUi& session = context.session;

	// TODO: Should logic be in here? Maybe it should be.
	//  Subviews should be split up though
	//  Each subview needs to be able to switch around ?
	if (session.running)
	{
		if (Navigation::BeginNavigation(context.theme, "RadioNav"))
		{
			Navigation::BeginNavList();
			RenderNavigation(context);
			Navigation::BeginCenter();
			RenderCenter(context.theme);

			Navigation::EndNav();
		}
	}
	else
	{
		RenderCenter(context.theme);
	}
}

void RadioView::RenderNavigation(const RenderContext &theme)
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
	// TODO: make better styling
	// switch (session_->GetStatus())
	// {
	// 	case Playing:
	// 	{
	// 		RenderRadio(theme);
	// 		break;
	// 	}
	// 	case Waiting:
	// 	{
	// 		ImGui::Text("Tuner not selected");
	// 		break;
	// 	}
	// 	case Stopping:
	// 	{
	// 		ImGui::Text("Stopping");
	// 		break;
	// 	}
	// 	case Starting:
	// 	{
	// 		ImGui::Text("Starting");
	// 		break;
	// 	}
	// 	case Error:
	// 	{
	// 		ImGui::Text("Error");
	// 		break;
	// 	}
	// 	default:
	// 		break;
	// }
}

void RadioView::RenderRadio(RenderContext &theme)
{
	if (current_page_ == Home)
		radio_home_.RenderCenter(theme);
	else if (radio_back_.RenderCenter(theme))
		current_page_ = Home;
}
