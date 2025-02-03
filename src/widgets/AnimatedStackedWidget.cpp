//
// Created by TheDaChicken on 8/17/2024.
//

#include <QParallelAnimationGroup>
#include <QTimer>
#include <QGraphicsOpacityEffect>

#include "AnimatedStackedWidget.h"

AnimatedStackedWidget::AnimatedStackedWidget(QWidget *parent) : QStackedWidget(parent)
{

}

/**
 * @brief Slide in the next widget in the stack
 * 	& slide out the current widget
 * @param next
 * @param params
 */
void AnimatedStackedWidget::SlideInWidget(QWidget *next, const AnimationParams &params,
                                          bool slide_in, bool slide_out)
{
	SlideDirection direction = params.direction;

	int iOffsetX = frameRect().width();
	int iOffsetY = frameRect().height();

	QWidget *prev = currentWidget();
	QPoint prevPos;

	if (prev)
	{
		prevPos = prev->pos();

		prev->clearFocus(); // Clear the focus from the current widget
	}

	next->raise(); // Raise the widget to the top
	next->show(); // Show the widget

	switch (direction)
	{
		case LeftToRight:
		{
			iOffsetY = 0;
			break;
		}
		case RightToLeft:
		{
			iOffsetX = -iOffsetX;
			iOffsetY = 0;
			break;
		}
		case TopToBottom:
		{
			iOffsetX = 0;
			break;
		}
		case BottomToTop:
		{
			iOffsetY = -iOffsetY;
			iOffsetX = 0;
			break;
		}
		default: break;
	}

	// Property Animation to slide in the next widget
	auto *anGroup = new QParallelAnimationGroup(this);

	if (prev)
	{
		// Slide out the current widget
		if (slide_out)
		{
			auto *slideOut = new QPropertyAnimation(prev, "pos", this);

			slideOut->setDuration(params.duration);
			slideOut->setEasingCurve(params.curve);
			slideOut->setStartValue(prevPos);
			slideOut->setEndValue(prevPos + QPoint(iOffsetX, iOffsetY));

			anGroup->addAnimation(slideOut);
		}

		// Fade out the current widget
		anGroup->addAnimation(CreateFadeAnimation(prev, params, false));
	}

	// Slide in the next widget
	if (slide_in)
	{
		auto *slideIn = new QPropertyAnimation(next, "pos", this);

		slideIn->setDuration(params.duration);
		slideIn->setEasingCurve(params.curve);

		slideIn->setStartValue(next->pos() - QPoint(iOffsetX, iOffsetY));
		slideIn->setEndValue(next->pos());

		anGroup->addAnimation(slideIn);
	}

	anGroup->addAnimation(CreateFadeAnimation(next, params, true));

	connect(anGroup,
	        &QParallelAnimationGroup::finished,
	        [this, anGroup, next, prev, prevPos]
	        {
		        setCurrentWidget(next);

		        if (prev)
		        {
			        prev->move(prevPos);
			        prev->hide();
		        }

		        anGroup->deleteLater();
	        });

	anGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

QPropertyAnimation *AnimatedStackedWidget::CreateFadeAnimation(QWidget *widget,
                                                               const AnimationParams &params,
                                                               bool in)
{
	auto effect = new QGraphicsOpacityEffect(this);
	auto fade = new QPropertyAnimation(effect, "opacity", this);

	fade->setDuration(params.duration);
	fade->setEasingCurve(params.curve);

	if (in)
	{
		effect->setOpacity(0.0);

		fade->setStartValue(0.0);
		fade->setEndValue(1.0);
	}
	else
	{
		fade->setStartValue(1.0);
		fade->setEndValue(0.0);
	}

	connect(fade,
	        &QPropertyAnimation::finished,
	        [effect]
	        {
		        effect->deleteLater();
	        });

	return fade;
}
