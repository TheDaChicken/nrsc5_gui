//
// Created by TheDaChicken on 7/29/2024.
//

#include "RadioDashboard.h"
#include "Application.h"
#include "utils/Error.h"

RadioDashboard::RadioDashboard(QWidget *parent)
	: DualViewWidget(parent),
	  state_(NRSC5_TUNER_NOT_ACTIVE),
	  main_view(new RadioMainView()),
	  control_panel_view(new RadioControlPanelView()),
	  status_view(new DualViewContainer(new LoadingPage(this), nullptr, nullptr))
{
	AddWidget(control_panel_view);
	AddWidget(main_view);
	AddWidget(status_view);

	connect(main_view->Header()->ImageLabel(),
	        &ImageTextLabel::clicked,
	        this, &RadioDashboard::ShowControlPanel);
	connect(control_panel_view->GetStationInfoPanel(),
	        &StationInfoPanel::clicked,
	        this, &RadioDashboard::SwitchToMain);
	connect(control_panel_view->GetFavoritesTree(),
	        &QListView::clicked,
	        this, &RadioDashboard::SwitchToMain);
	connect(control_panel_view->GetTuneWidget(),
	        &TuneWidget::TuneChanged,
	        this, &RadioDashboard::SwitchToMain);
}

RadioDashboard::~RadioDashboard() = default;

void RadioDashboard::Update() const
{
	// Update current view on RadioDashboard
	if (!IsStatusOk())
	{
		// If the tuner isn't loaded, show the status page to the user
		StatusPage()->SetStatus(StatusString(),
		                        NRSC5_TUNER_STARTING == state_ || NRSC5_TUNER_STOPPING == state_);
		SetCurrentWidget(status_view);
	}
	else
	{
		// If the tuner is loaded, show the main view to the user
		StatusPage()->SetStatus(StatusString(), false);
		SetCurrentWidget(main_view);
	}
}

void RadioDashboard::SwitchToMain() const
{
	SlideInNext(main_view,
	            {
		            300, QEasingCurve::OutCubic,
		            AnimatedStackedWidget::SlideDirection::BottomToTop
	            });
}

void RadioDashboard::ShowControlPanel() const
{
	SetCurrentWidget(control_panel_view);
}

QString RadioDashboard::StatusString() const
{
	// Return the status of the tuner as a string
	switch (state_)
	{
		/* not used */
		case NRSC5_TUNER_ACTIVE: return tr("Tuner is active");
		case NRSC5_TUNER_NOT_ACTIVE: return tr("Tuner is currently off");
		case NRSC5_TUNER_STARTING: return tr("Starting tuner");
		case NRSC5_TUNER_STOPPING: return tr("Stopping tuner");
		case NRSC5_SDR_NO_DEVICE: return tr("No SDR device found");
		case NRSC5_SDR_BAD_COMMUNICATION: return tr("Bad communication with SDR device");
		default:
		case NRSC5_DEFAULT_ERROR: return tr("Tuner encountered an error");
	}
}

bool RadioDashboard::IsStatusOk() const
{
	return state_ == NRSC5_TUNER_ACTIVE;
}
