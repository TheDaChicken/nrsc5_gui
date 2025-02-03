//
// Created by TheDaChicken on 8/23/2024.
//

#ifndef NRSC5_GUI_LIB_IMAGES_PROVIDERS_IIMAGEPROVIDER_H_
#define NRSC5_GUI_LIB_IMAGES_PROVIDERS_IIMAGEPROVIDER_H_

#include "images/ImageData.h"
#include "RadioChannel.h"

class IImageProvider
{
 public:
  virtual ~IImageProvider() = default;

  virtual ImageData FetchStationImage(const RadioChannel &channel) = 0;
  virtual ImageData FetchPrimaryImage(const RadioChannel &channel, const NRSC5::ID3& id3) = 0;
};

#endif //NRSC5_GUI_LIB_IMAGES_PROVIDERS_IIMAGEPROVIDER_H_
