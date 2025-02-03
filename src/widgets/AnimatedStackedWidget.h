//
// Created by TheDaChicken on 8/17/2024.
//

#ifndef NRSC5_GUI_SRC_WIDGET_ANIMATEDSTACKEDWIDGET_H_
#define NRSC5_GUI_SRC_WIDGET_ANIMATEDSTACKEDWIDGET_H_

#include <QStackedWidget>
#include <QPropertyAnimation>

/**
 * @brief A custom stacked widget that allows sliding in the next widget
 */
class AnimatedStackedWidget : public QStackedWidget
{
 Q_OBJECT
 public:
  enum SlideDirection
  {
	LeftToRight,
	RightToLeft,
	TopToBottom,
	BottomToTop
  };
  struct AnimationParams
  {
	int duration = 500;
	QEasingCurve curve;
	SlideDirection direction = LeftToRight;
  };

  explicit AnimatedStackedWidget(QWidget *parent = nullptr);

  void SlideInWidget(QWidget *next, const AnimationParams &params, bool slide_in = true, bool slide_out = true);
 private:
 QPropertyAnimation *CreateFadeAnimation(QWidget *widget, const AnimationParams &params, bool in = true);
};

#endif //NRSC5_GUI_SRC_WIDGET_ANIMATEDSTACKEDWIDGET_H_
