//
// Created by TheDaChicken on 8/17/2024.
//

#include "DualViewWidget.h"

DualViewWidget::DualViewWidget(QWidget *parent) : QFrame(parent)
{
	layout_ = new DualViewLayout(this);

	left_widget_ = new AnimatedStackedWidget(this);
	header_widget_ = new AnimatedStackedWidget(this);
	main_widget_ = new AnimatedStackedWidget(this);

	left_widget_->setObjectName("LeftWidget");
	header_widget_->setObjectName("HeaderWidget");
	main_widget_->setObjectName("MainWidget");

	layout_->AddHeaderWidget(header_widget_);
	layout_->AddLeftWidget(left_widget_);
	layout_->AddMainWidget(main_widget_);
}

void DualViewWidget::AddWidget(const DualViewContainer *widget) const
{
	Q_ASSERT(widget->GetMainWidget()); // Main widget must exist

	// Add the main widget
	main_widget_->addWidget(widget->GetMainWidget());

	widget->GetMainWidget()->setAutoFillBackground(true);

	// Add the left and header widgets if they exist
	if (widget->GetLeftWidget())
	{
		left_widget_->addWidget(widget->GetLeftWidget());

		widget->GetLeftWidget()->setAutoFillBackground(true);
	}
	if (widget->GetHeaderWidget())
	{
		header_widget_->addWidget(widget->GetHeaderWidget());
	}
}

void DualViewWidget::SetCurrentWidget(const DualViewContainer *widget) const
{
	SetStackWidget(main_widget_, widget->GetMainWidget());
	SetStackWidget(left_widget_, widget->GetLeftWidget());
	SetStackWidget(header_widget_, widget->GetHeaderWidget());
}

void DualViewWidget::SlideInNext(const DualViewContainer *widget,
                                 const AnimatedStackedWidget::AnimationParams &main_widget_params) const
{
	if (!widget)
		return;

	// Slide up the new main widget
	main_widget_->SlideInWidget(widget->GetMainWidget(), main_widget_params, true, false);

	// Slide Right to Left in new Left Widget
	SlideInWidget(left_widget_,
	              widget->GetLeftWidget(),
	              {
		              main_widget_params.duration, QEasingCurve::OutCubic,
		              AnimatedStackedWidget::SlideDirection::RightToLeft
	              });

	// Don't slide in the left widget header
	SetStackWidget(header_widget_, widget->GetHeaderWidget());
}

void DualViewWidget::SetStackWidget(AnimatedStackedWidget *stacked, QWidget *widget)
{
	assert(stacked != nullptr);

	if (widget == nullptr)
	{
		// Hide the stacked widget if the widget is null
		stacked->hide();
	}
	else
	{
		stacked->show();
		stacked->setCurrentWidget(widget);
	}
}

void DualViewWidget::SlideInWidget(AnimatedStackedWidget *stacked, QWidget *widget,
                                   const AnimatedStackedWidget::AnimationParams &params)
{
	assert(stacked != nullptr);

	if (widget == nullptr)
	{
		// Hide the stacked widget if the widget is null
		stacked->hide();
	}
	else
	{
		stacked->show();
		stacked->SlideInWidget(widget, params);
	}
}
