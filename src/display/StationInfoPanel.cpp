//
// Created by TheDaChicken on 8/15/2024.
//

#include "StationInfoPanel.h"

#include <Application.h>
#include <widgets/text/ImageTextEdit.h>

#include "utils/StylingText.h"
#include "utils/Log.h"

StationInfoPanel::StationInfoPanel(QWidget *parent) : QFrame(parent)
{
  setObjectName("StationInfoBar");
  setFrameShape(StyledPanel);
  setFrameShadow(Plain);
  setMinimumSize(QSize(100, 100));
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  setContentsMargins(15, 6, 15, 6);

  layout_ = new QHBoxLayout(this);
  layout_->setObjectName("StationInfoLayout");

  // Add the Station Info Label to the layout
  station_info_label_ = new ImageTextEdit(this);
  station_info_label_->setObjectName("StationInfoLabel");
  station_info_label_->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);
  station_info_label_->setFrameShadow(Plain);
  station_info_label_->setFrameShape(NoFrame);
  station_info_label_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  layout_->addWidget(station_info_label_);

  // Add the HD Small Icon to the layout
  hd_small_icon_ = new ImageTextLabel(this);
  hd_small_icon_->setObjectName("HDSmallIcon");
  hd_small_icon_->setMinimumSize(QSize(100, 40));
  hd_small_icon_->setFrameShadow(Plain);
  hd_small_icon_->setFrameShape(NoFrame);

  layout_->addWidget(hd_small_icon_);

  // Connect the StationInfoPanel to the StationInfoManager
  connect(&getApp()->GetStationInfoManager(),
          &StationInfoManager::UpdateActiveChannel,
          this,
          &StationInfoPanel::DisplayStation);
}

/**
 * @brief Displays Channel by styling text from RadioChannel
 * @param channel RadioChannel to display
 */
void StationInfoPanel::DisplayStation(const ActiveChannel &channel) const
{
  StylingText::DisplayStation(
    station_info_label_->document(),
    channel,
    StylingText::Direction::SHORT);
  //hd_small_icon_->setPixmap(channel.GetHDIcon());
  //hd_small_icon_->setDescription(QString::number(channel.program_id_));
}

void StationInfoPanel::mousePressEvent(QMouseEvent *event)
{
  emit clicked();
  QWidget::mousePressEvent(event);
}
