//
// Created by TheDaChicken on 8/23/2024.
//

#ifndef NRSC5_GUI_LIB_IMAGES_PROVIDERS_LOTIMAGEPROVIDER_H_
#define NRSC5_GUI_LIB_IMAGES_PROVIDERS_LOTIMAGEPROVIDER_H_

#include "IImageProvider.h"
#include "nrsc5/LotManager.h"

/**
 * Handles providing lot images.
 */
class LotImageProvider : public IImageProvider
{
 public:
  explicit LotImageProvider(LotManager *lot_manager);

  ImageData FetchStationImage(const Channel &channel) override;
  ImageData FetchPrimaryImage(const Channel &channel, const NRSC5::ID3& id3) override;

  static ImageData LoadLotImage(const NRSC5::Lot &lot);
 private:
  LotManager *lot_manager_;
};

#endif //NRSC5_GUI_LIB_IMAGES_PROVIDERS_LOTIMAGEPROVIDER_H_
