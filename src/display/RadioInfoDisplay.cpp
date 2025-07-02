//
// Created by TheDaChicken on 8/8/2024.
//

#include "RadioInfoDisplay.h"

#include <Application.h>
#include <controllers/StationInfoManager.h>

#include "nrsc5/Station.h"
#include "utils/StylingText.h"

#include "ui_RadioInfoDisplay.h"

RadioInfoDisplay::RadioInfoDisplay(QWidget *parent)
  : QFrame(parent), ui(new Ui::RadioCoverLayout)
{
  ui->setupUi(this);
  ui->PrimaryLogo->setPixmap(getApp()->GetImageProvider().DefaultImage());
  ui->PrimaryLogo->setAlignment(ImageText::TextRight | ImageText::CenterX);
  ui->PrimaryLogo->clear();

  ui->RadioLogo->setPixmap(getApp()->GetImageProvider().DefaultLogo());
  ui->RadioLogo->setAlignment(ImageText::TextRight | ImageText::CenterX);
  ui->RadioLogo->clear();

  ui->HDLogo->setAlignment(ImageText::TextBottom | ImageText::CenterX);
  ui->HDLogo->hide();

  ui->lower->hide();
  ui->upper->hide();

  // Connect the buttons
  connect(ui->PlusButton, &QPushButton::clicked, this, &RadioInfoDisplay::OnPlusButton);
  connect(ui->MinusButton, &QPushButton::clicked, this, &RadioInfoDisplay::OnMinusButton);
  connect(ui->FavoriteButton, &QPushButton::clicked, this, &RadioInfoDisplay::OnFavoriteButton);

  // Connect this class to the StationInfoManager
  connect(&getApp()->GetStationInfoManager(),
          &StationInfoManager::UpdateActiveChannel,
          this,
          &RadioInfoDisplay::DisplayChannel);
  connect(&getApp()->GetStationInfoManager(),
          &StationInfoManager::UpdateStationLogo,
          this,
          &RadioInfoDisplay::DisplayStationLogo);
  connect(&getApp()->GetStationInfoManager(),
          &StationInfoManager::UpdatePrimaryImage,
          this,
          &RadioInfoDisplay::DisplayPrimaryImage);
  connect(&getApp()->GetStationInfoManager(),
          &StationInfoManager::UpdateHDSync,
          this,
          &RadioInfoDisplay::DisplayHDSync);
  connect(&getApp()->GetStationInfoManager(),
          &StationInfoManager::UpdateID3,
          this,
          &RadioInfoDisplay::DisplayHDID3);
  connect(&getApp()->GetStationInfoManager(),
          &StationInfoManager::UpdateFavorite,
          this,
          &RadioInfoDisplay::DisplayFavorite);
  connect(&getApp()->GetStationInfoManager(),
          &StationInfoManager::ClearId3,
          this,
          &RadioInfoDisplay::ClearID3);
  connect(&getApp()->GetRadioController(),
          &HybridRadio::HDSignalStrength,
          this,
          &RadioInfoDisplay::DisplaySignalMeter);
}

RadioInfoDisplay::~RadioInfoDisplay()
{
  delete ui;
}

void RadioInfoDisplay::DisplayStationLogo(const QPixmap &logo) const
{
  ui->RadioLogo->setImage(logo.toImage());
}

void RadioInfoDisplay::DisplayChannel(const ActiveChannel &channel) const
{
  StylingText::DisplayStation(ui->RadioLogo->document(),
                              channel,
                              StylingText::Direction::LONG);
  StylingText::GenerateChannelList(ui->HDLogo->document(),
                                   channel.hd_details.programs,
                                   channel.station_info.current_program);
}

void RadioInfoDisplay::DisplayHDID3(const NRSC5::ID3 &id3) const
{
  StylingText::DisplayID3(ui->PrimaryLogo->document(), id3);
}

void RadioInfoDisplay::ClearID3() const
{
  ui->PrimaryLogo->clear();
}

void RadioInfoDisplay::DisplayPrimaryImage(const QPixmap &image) const
{
  ui->PrimaryLogo->setImage(image.toImage());
}

void RadioInfoDisplay::DisplayFavorite(const QModelIndex &index) const
{
  ui->FavoriteButton->setChecked(index.isValid());
}

void RadioInfoDisplay::DisplaySignalMeter(float lower, float upper)
{
  ui->lower->viewport()->setAutoFillBackground(false);
  ui->upper->viewport()->setAutoFillBackground(false);

  StylingText::DisplaySignalStrength(ui->lower->document(), lower);
  StylingText::DisplaySignalStrength(ui->upper->document(), upper);
}

void RadioInfoDisplay::DisplayHDSync(bool sync) const
{
  if (sync)
  {
    ui->HDLogo->show();
    //ui->lower->show();
    //ui->upper->show();
  }
  else
  {
    ui->HDLogo->hide();
    ui->lower->hide();
    ui->upper->hide();
  }
}

void RadioInfoDisplay::OnPlusButton()
{
  Q_UNUSED(this)

  const ActiveChannel &channel = getApp()->GetRadioController().GetChannel();
  auto itr = channel.hd_details.programs.upper_bound(channel.station_info.current_program);

  if (itr != channel.hd_details.programs.end())
  {
    getApp()->GetRadioController().SetProgram(itr->first);
  }
  else
  {
    // TODO: Scan for next Station
  }
}

void RadioInfoDisplay::OnMinusButton()
{
  Q_UNUSED(this)

  const ActiveChannel &channel = getApp()->GetRadioController().GetChannel();
  auto itr = channel.hd_details.programs.lower_bound(channel.station_info.current_program);

  if (itr != channel.hd_details.programs.begin())
  {
    --itr;
    getApp()->GetRadioController().SetProgram(itr->first);
  }
  else
  {
    // TODO: Scan for next Station
  }
}

void RadioInfoDisplay::OnFavoriteButton(bool status)
{
  const ActiveChannel &channel = getApp()->GetRadioController().GetChannel();

  if (status)
  {
    if (!getApp()->GetFavoritesModel()->Add(channel))
    {
      // Failed to add channel
      ui->FavoriteButton->setChecked(false);
    }
  }
  else
  {
    getApp()->GetFavoritesModel()->Remove(channel);
  }
}

//
///**
// * @brief Meant to uncheck the favorite button if the current channel was removed from favorites.
// * @param channel
// */
//void RadioInfoDisplay::OnFavoriteRemoved(const QModelIndex &parent, int first, int last)
//{
//  Q_UNUSED(parent)
//  const auto *dApp = dynamic_cast<dApp *>(QdApp::instance());
//
//  for (int i = first; i <= last; i++)
//  {
//	if (dApp->GetRadioChannel() == favorites_->GetActiveChannel(i))
//	{
//	  // The current channel was removed from favorites
//	  // Uncheck the favorite button
//	  ui->FavoriteButton->setChecked(false);
//	  return;
//	}
//  }
//}
//
///**
// * @brief Meant to check the favorite button if the current channel was added to favorites.
// * @param channel
// */
//void RadioInfoDisplay::OnFavoriteAdded(const QModelIndex &parent, int first, int last)
//{
//  Q_UNUSED(parent)
//  const auto *dApp = dynamic_cast<dApp *>(QdApp::instance());
//
//  for (int i = first; i <= last; i++)
//  {
//	Logger::Log(info, "Checking favorite button for channel: {}", favorites_->GetActiveChannel(i).GetDisplayChannel());
//	if (dApp->GetRadioChannel() == favorites_->GetActiveChannel(i))
//	{
//	  // The current channel was added to favorites
//	  // Check the favorite button
//	  ui->FavoriteButton->setChecked(true);
//	  return;
//	}
//  }
//}

