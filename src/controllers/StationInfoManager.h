//
// Created by TheDaChicken on 8/22/2024.
//

#ifndef NRSC5_GUI_SRC_STATIONINFOMANAGER_H_
#define NRSC5_GUI_SRC_STATIONINFOMANAGER_H_

#include <QFuture>

#include "models/FavoriteModel.h"
#include "images/StationImageProvider.h"
#include "nrsc5/Station.h"
#include "RadioChannel.h"

/**
 * Handles Station information
 * Esp. HDRadio provided information (Station Logo / ID3 information) to be displayed on the UI
 */
class StationInfoManager : public QObject
{
  Q_OBJECT

 public:
  explicit StationInfoManager(const std::weak_ptr<StationImageProvider> &image_provider,
                              const std::weak_ptr<FavoriteModel> &favorites);

  void ReceiveLot(const NRSC5::DataService &component, const NRSC5::Lot &lot);

  void StyleAndDisplayStation(const ActiveChannel &channel);
  void StyleAndDisplayID3(const NRSC5::ID3 &id3);

  void DisplayPrimaryImage(const ImageData &image);
  void DisplayFavorite(const Channel &channel);

  /**
   * @brief Display the station logo for primary or fallback primary
   *
   */
  void DisplayFallbackPrimaryImage();
  /**
   * @brief Clear the ID3 information
   *
   */
  void ClearID3();

 signals:
  /**
   * @brief Signals HDRadio sync status.
   * @param sync
   */
  void UpdateHDSync(bool sync);
  /**
   * @brief Update the channel information.
   * @param channel The radio channel.
   */
  void UpdateActiveChannel(const ActiveChannel &channel);
  /**
   * @brief Provides new station logo.
   * This runs on every station change.
   * @param pixmap The station logo.
   */
  void UpdateStationLogo(const QPixmap &pixmap);
  /**
   * @brief Gives favorite status of the channel.
   * This runs on every station change.
   * @param index favorites list index.
   */
  void UpdateFavorite(const QModelIndex &index);
  /**
   * @brief Clear the Artist/Title information.
   * This runs on every station change.
   */
  void ClearId3();
  /**
   * @brief Provides current ID3 information.
   * @param id3 The ID3 information.
   */
  void UpdateID3(const NRSC5::ID3 &id3);

  /**
   * @brief Update the primary image.
   * @param pixmap
   */
  void UpdatePrimaryImage(const QPixmap &pixmap);
 private:
  void FetchPrimaryImage(const NRSC5::ID3 &id3);

  QFuture<ImageData> FetchPrimaryImage(const Channel &channel, const NRSC5::ID3 &id3) const;
  [[nodiscard]] ImageData FallbackPrimaryImage() const;

  // Station information
  NRSC5::StationInfo station_;
  NRSC5::ID3 station_id3_;

  ImageData station_logo_;
  ImageData primary_image_;

  std::weak_ptr<StationImageProvider> image_provider_;
  std::weak_ptr<FavoriteModel> favorites_;

  QFuture<ImageData> primary_image_future_;
};

#endif //NRSC5_GUI_SRC_STATIONINFOMANAGER_H_
