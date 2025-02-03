//
// Created by TheDaChicken on 9/5/2024.
//

#ifndef NRSC5_GUI_SRC_WIDGET_FRAMEPANEL_H_
#define NRSC5_GUI_SRC_WIDGET_FRAMEPANEL_H_

#include <QFrame>
#include <QScrollArea>

#include "display/StationInfoPanel.h"

/**
 * A QFrame with ScrollArea & StationInfoPanel on the bottom
 */
class StationFramePanel : public QFrame
{
 public:
  explicit StationFramePanel(QWidget *parent = nullptr);

  [[nodiscard]] StationInfoPanel *GetStationInfoPanel() const
  {
   return station_info_panel_;
  }

  [[nodiscard]] QFrame *GetScrollAreaFrame() const
  {
   return scroll_area_frame_;
  }

  void AddWidget(QWidget *widget) const
  {
   scroll_area_layout_->addWidget(widget);
  }

 private:
  QVBoxLayout *layout_;
  QScrollArea *scroll_area_;
  QFrame *scroll_area_frame_;
  QVBoxLayout *scroll_area_layout_;
  StationInfoPanel *station_info_panel_;
};

#endif //NRSC5_GUI_SRC_WIDGET_FRAMEPANEL_H_
