//
// Created by TheDaChicken on 8/15/2024.
//

#ifndef NRSC5_GUI_SRC_WIDGET_STATIONINFOPANEL_H_
#define NRSC5_GUI_SRC_WIDGET_STATIONINFOPANEL_H_

#include <QHBoxLayout>
#include <QTextEdit>

#include "RadioChannel.h"
#include "widgets/text/ImageTextLabel.h"
#include "widgets/text/ImageTextEdit.h"

/**
 * @brief A widget that vertically displays content and StationInfoBar
 * Example: Used to display: FavoritesList List and StationInfoBar
 */
class StationInfoPanel : public QFrame
{
  Q_OBJECT

 public:
  explicit StationInfoPanel(QWidget *parent = nullptr);

  void DisplayStation(const ActiveChannel &channel) const;
 signals:
  void clicked();

 private:
  void mousePressEvent(QMouseEvent *event) override;

  //  QVBoxLayout *main_layout_;
  //  QScrollArea *scroll_area_;
  //  QWidget *scroll_area_widget;
  //  QVBoxLayout *scroll_area_layout;

  QHBoxLayout *layout_;
  ImageTextEdit *station_info_label_;
  ImageTextLabel *hd_small_icon_;
};

#endif //NRSC5_GUI_SRC_WIDGET_STATIONINFOPANEL_H_
