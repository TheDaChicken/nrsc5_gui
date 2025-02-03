//
// Created by TheDaChicken on 8/17/2024.
//

#ifndef NRSC5_GUI_SRC_WIDGET_DUALWIDGETCONTAINER_H_
#define NRSC5_GUI_SRC_WIDGET_DUALWIDGETCONTAINER_H_

#include <QWidget>

/**
 * Container for widgets that can be placed on DualViewWidget
 */
class DualViewContainer : public QObject
{
 Q_OBJECT
 public:
  explicit DualViewContainer()
	  : main_widget_(nullptr),
		left_widget_(nullptr),
		header_widget_(nullptr)
  {
  }

  explicit DualViewContainer(QWidget *main_widget, QWidget *left_widget, QWidget *header_widget)
	  : main_widget_(main_widget),
		left_widget_(left_widget),
		header_widget_(header_widget)
  {
  }

  void SetMainWidget(QWidget *widget)
  {
	main_widget_ = widget;
  }

  void SetLeftWidget(QWidget *widget)
  {
	left_widget_ = widget;
  }

  void SetLeftWidgetHeader(QWidget *widget)
  {
	header_widget_ = widget;
  }

  [[nodiscard]] QWidget *GetMainWidget() const
  {
	return main_widget_;
  }

  [[nodiscard]] QWidget *GetLeftWidget() const
  {
	return left_widget_;
  }

  [[nodiscard]] QWidget *GetHeaderWidget() const
  {
	return header_widget_;
  }
 private:
  QWidget *main_widget_;
  QWidget *left_widget_;
  QWidget *header_widget_;
};

#endif //NRSC5_GUI_SRC_WIDGET_DUALWIDGETCONTAINER_H_
