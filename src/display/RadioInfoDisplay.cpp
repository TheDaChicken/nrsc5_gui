//
// Created by TheDaChicken on 8/8/2024.
//

#include "RadioInfoDisplay.h"

#include <Application.h>
#include <controllers/StationInfoManager.h>

#include "nrsc5/Station.h"
#include "utils/StylingText.h"

#include "ui_RadioInfoDisplay.h"

#define HDRADIO_LOGO ":/images/HDRadioLogo.svg"

RadioInfoDisplay::RadioInfoDisplay(QWidget *parent)
  : QFrame(parent), ui(new Ui::RadioCoverLayout)
{
  ui->setupUi(this);
  ui->PrimaryLogo->setPixmap(dApp->GetImageProvider().DefaultImage());
  ui->PrimaryLogo->setAlignment(ImageText::TextRight | ImageText::CenterX);
  ui->PrimaryLogo->clear();

  ui->RadioLogo->setPixmap(dApp->GetImageProvider().DefaultRadio());
  ui->RadioLogo->setAlignment(ImageText::TextRight | ImageText::CenterX);
  ui->RadioLogo->clear();

  ui->HDLogo->setAlignment(ImageText::TextBottom | ImageText::CenterX);
  ui->HDLogo->setPixmap(QPixmap(HDRADIO_LOGO));
  ui->HDLogo->hide();

  ui->lower->hide();
  ui->upper->hide();

  // Connect the buttons
  connect(ui->PlusButton, &QPushButton::clicked, this, &RadioInfoDisplay::OnPlusButton);
  connect(ui->MinusButton, &QPushButton::clicked, this, &RadioInfoDisplay::OnMinusButton);
  connect(ui->FavoriteButton, &QPushButton::clicked, this, &RadioInfoDisplay::OnFavoriteButton);

  // Connect this class to the StationInfoManager
  connect(&dApp->GetStationInfoManager(),
          &StationInfoManager::UpdateChannel,
          this,
          &RadioInfoDisplay::DisplayChannel);
  connect(&dApp->GetStationInfoManager(),
          &StationInfoManager::UpdateStationLogo,
          this,
          &RadioInfoDisplay::DisplayStationLogo);
  connect(&dApp->GetStationInfoManager(),
        &StationInfoManager::UpdatePrimaryImage,
        this,
        &RadioInfoDisplay::DisplayPrimaryImage);
  connect(&dApp->GetStationInfoManager(),
          &StationInfoManager::UpdateHDSync,
          this,
          &RadioInfoDisplay::DisplayHDSync);
  connect(&dApp->GetStationInfoManager(),
          &StationInfoManager::UpdateID3,
          this,
          &RadioInfoDisplay::DisplayHDID3);
  connect(&dApp->GetStationInfoManager(),
          &StationInfoManager::UpdateFavorite,
          this,
          &RadioInfoDisplay::DisplayFavorite);
  connect(&dApp->GetStationInfoManager(),
          &StationInfoManager::ClearId3,
          this,
          &RadioInfoDisplay::ClearID3);
  connect(&dApp->GetRadioController(),
        &RadioController::HDSignalStrength,
        this,
        &RadioInfoDisplay::DisplaySignalMeter);
}

RadioInfoDisplay::~RadioInfoDisplay()
{
  delete ui;
}

void RadioInfoDisplay::DisplayStationLogo(const QPixmap &logo) const
{
  ui->RadioLogo->setPixmap(logo);
}

void RadioInfoDisplay::DisplayChannel(const RadioChannel &channel) const
{
  const NRSC5::Station &station = channel.hd_station_;

  StylingText::DisplayStation(ui->RadioLogo->document(),
                              channel,
                              StylingText::Direction::LONG);
  StylingText::GenerateChannelList(ui->HDLogo->document(),
                                   station.programs,
                                   station.current_program);
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
  ui->PrimaryLogo->setPixmap(image);
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

  const RadioChannel &kRadioChannel = dApp->GetRadioController().GetChannel();

  unsigned int nextProgram = kRadioChannel.hd_station_.current_program + 1;

  if (nextProgram >= kRadioChannel.hd_station_.programs.size())
  {
    // TODO: Scan for next Station
  }
  else
  {
    dApp->GetRadioController().SetProgram(nextProgram);
  }
}

void RadioInfoDisplay::OnMinusButton()
{
  Q_UNUSED(this)

  const RadioChannel &kRadioChannel = dApp->GetRadioController().GetChannel();

  if (kRadioChannel.hd_station_.current_program <= 0)
  {
    // TODO: Scan for next Station
  }
  else
  {
    unsigned int nextProgram = kRadioChannel.hd_station_.current_program - 1;
    dApp->GetRadioController().SetProgram(nextProgram);
  }
}

void RadioInfoDisplay::OnFavoriteButton(bool status)
{
  const RadioChannel &kRadioChannel = dApp->GetRadioController().GetChannel();

  if (status)
  {
    if (!dApp->GetFavoritesModel()->AddChannel(kRadioChannel))
    {
      // Failed to add channel
      ui->FavoriteButton->setChecked(false);
    }
  }
  else
  {
    dApp->GetFavoritesModel()->RemoveChannel(kRadioChannel);
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
//	if (dApp->GetRadioChannel() == favorites_->GetChannel(i))
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
//	Logger::Log(info, "Checking favorite button for channel: {}", favorites_->GetChannel(i).GetDisplayChannel());
//	if (dApp->GetRadioChannel() == favorites_->GetChannel(i))
//	{
//	  // The current channel was added to favorites
//	  // Check the favorite button
//	  ui->FavoriteButton->setChecked(true);
//	  return;
//	}
//  }
//}

