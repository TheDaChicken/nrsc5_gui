//
// Created by TheDaChicken on 9/5/2024.
//

#include "StationFramePanel.h"

StationFramePanel::StationFramePanel(QWidget *parent) : QFrame(parent)
{
  setFrameStyle(Plain);
  setFrameShape(NoFrame);

  layout_ = new QVBoxLayout(this);
  layout_->setContentsMargins(0, 0, 0, 0);
  layout_->setSpacing(0);

  scroll_area_ = new QScrollArea(this);
  scroll_area_->setWidgetResizable(true);
  scroll_area_->setFrameShadow(Plain);
  scroll_area_->setFrameShape(NoFrame);
  scroll_area_->setObjectName("Stacked ScrollArea");

  scroll_area_frame_ = new QFrame(scroll_area_);
  scroll_area_frame_->setContentsMargins(9, 9, 9, 9);
  scroll_area_frame_->setFrameStyle(NoFrame);
  scroll_area_frame_->setFrameShadow(Plain);
  scroll_area_frame_->setObjectName("ScrollArea Frame");

  scroll_area_layout_ = new QVBoxLayout(scroll_area_frame_);

  scroll_area_->setWidget(scroll_area_frame_);
  scroll_area_->viewport()->setAutoFillBackground(false);

  layout_->addWidget(scroll_area_);

  station_info_panel_ = new StationInfoPanel(this);

  layout_->addWidget(station_info_panel_);

  layout_->setStretch(0, 1);
  layout_->setStretch(1, 0);
}
