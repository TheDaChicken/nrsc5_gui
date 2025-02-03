//
// Created by TheDaChicken on 8/17/2024.
//

#ifndef NRSC5_GUI_SRC_WIDGET_DOUBLESTACKEDWIDGET_H_
#define NRSC5_GUI_SRC_WIDGET_DOUBLESTACKEDWIDGET_H_

#include "display/StationInfoPanel.h"
#include "widgets/AnimatedStackedWidget.h"
#include "DualViewLayout.h"
#include "DualViewContainer.h"

/**
 * @brief A multi-view widget that allows you to switch out specific widgets
 * This encapsulates three stacked widgets
 *
 * Layout is as follows:
 *
 * Header describing the current controller
 * ListView Widget | Main Widget
 *
 */
class DualViewWidget : public QFrame
{
 public:
  explicit DualViewWidget(QWidget *parent);

  void AddWidget(const DualViewContainer *widget) const;
  void SetCurrentWidget(const DualViewContainer *widget) const;
  void SlideInNext(const DualViewContainer *widget, const AnimatedStackedWidget::AnimationParams &main_widget_params) const;

  [[nodiscard]] AnimatedStackedWidget *GetMainWidget() const
  {
	return main_widget_;
  }

  [[nodiscard]] AnimatedStackedWidget *GetLeftWidget() const
  {
	return left_widget_;
  }

  [[nodiscard]] AnimatedStackedWidget *GetLeftWidgetHeader() const
  {
	return header_widget_;
  }

 private:
  static void SetStackWidget(AnimatedStackedWidget *stacked, QWidget *widget);
  static void SlideInWidget(AnimatedStackedWidget *stacked, QWidget *widget,
							const AnimatedStackedWidget::AnimationParams &params);

  DualViewLayout* layout_;

  AnimatedStackedWidget* left_widget_;
  AnimatedStackedWidget* header_widget_;
  AnimatedStackedWidget* main_widget_;
};

#endif //NRSC5_GUI_SRC_WIDGET_DOUBLESTACKEDWIDGET_H_
