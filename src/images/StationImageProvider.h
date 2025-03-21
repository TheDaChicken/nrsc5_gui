//
// Created by TheDaChicken on 8/22/2024.
//

#ifndef NRSC5_GUI_LIB_STATIONIMAGEPROVIDER_H_
#define NRSC5_GUI_LIB_STATIONIMAGEPROVIDER_H_

#include "nrsc5/LotManager.h"
#include "images/providers/IImageProvider.h"
#include "ImageData.h"
#include "ImageProviderPriorityManager.h"

/**
 * Handles providing station images.
 */
class StationImageProvider
{
 public:
  explicit StationImageProvider();

  void AddProvider(const std::shared_ptr<IImageProvider> &provider, int priority) const;

  [[nodiscard]] ImageData FetchStationImage(const Channel &channel) const;
  [[nodiscard]] ImageData FetchPrimaryImage(const Channel &channel, const NRSC5::ID3 &id3) const;

  [[nodiscard]] ImageData MissingRadio() const;
  [[nodiscard]] ImageData MissingImage() const;

  [[nodiscard]] QPixmap DefaultImage() const
  {
   return default_image_;
  }

  [[nodiscard]] QPixmap DefaultRadio() const
  {
   return default_radio_;
  }

  [[nodiscard]] ImageProviderPriorityManager *ProviderManager() const
  {
   return provider_manager_.get();
  }

 private:
  QPixmap default_image_;
  QPixmap default_radio_;

  std::unique_ptr<ImageProviderPriorityManager> provider_manager_;
};

#endif //NRSC5_GUI_LIB_STATIONIMAGEPROVIDER_H_
