//
// Created by TheDaChicken on 7/29/2024.
//

#include "RadioPage.h"
#include "Application.h"

RadioPage::RadioPage(QWidget *parent)
	: DualViewWidget(parent),
	  main_view(new RadioMainView()),
	  control_panel_view(new RadioControlPanelView()),
	  status_view(new DualViewContainer(new LoadingPage(this), nullptr, nullptr))
{
	AddWidget(control_panel_view);
	AddWidget(main_view);
	AddWidget(status_view);

	connect(main_view->Header()->ImageLabel(),
	        &ImageTextLabel::clicked,
	        this,
	        &RadioPage::ShowControlPanel);
	connect(control_panel_view->GetStationInfoPanel(),
	        &StationInfoPanel::clicked,
	        this,
	        &RadioPage::SwitchToMain);
	connect(control_panel_view->GetFavoritesTree(),
	        &QListView::clicked,
	        this,
	        &RadioPage::SwitchToMain);
	connect(control_panel_view->GetTuneWidget(),
	        &TuneWidget::TuneChanged,
	        this,
	        &RadioPage::SwitchToMain);

	UpdateTunerStatus(TunerAction::Stopped, UTILS::StatusCodes::Ok);
}

RadioPage::~RadioPage() = default;

void RadioPage::UpdateTunerStatus(const TunerAction &action, const UTILS::StatusCodes &state) const
{
	if (state != UTILS::StatusCodes::Ok)
	{
		// If there was an error, show the error to the user
		StatusPage()->SetStatus(Application::GetStatusMessage(state), false);
		SetCurrentWidget(status_view);
		return;
	}

	if (action != TunerAction::Started)
	{
		StatusPage()->SetStatus(ActionString(action),
		                        TunerAction::Starting == action || TunerAction::Stopping == action);
		SetCurrentWidget(status_view);
	}
	else
	{
		StatusPage()->SetStatus(ActionString(action), false);
		SetCurrentWidget(main_view);
	}
}

void RadioPage::SwitchToMain() const
{
	SlideInNext(main_view,
	            {
		            300, QEasingCurve::OutCubic,
		            AnimatedStackedWidget::SlideDirection::BottomToTop
	            });
}

void RadioPage::ShowControlPanel() const
{
	SetCurrentWidget(control_panel_view);
}

QString RadioPage::ActionString(const TunerAction &action) const
{
	switch (action)
	{
		case TunerAction::Started:
			return tr("Tuner is active");
		case TunerAction::Stopped:
			return tr("Tuner is currently off");
		case TunerAction::Starting:
			return tr("Starting tuner");
		case TunerAction::Stopping:
			return tr("Stopping tuner");
		default:
			return "";
	}
}
