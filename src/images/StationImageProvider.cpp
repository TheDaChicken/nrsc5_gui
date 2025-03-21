//
// Created by TheDaChicken on 8/22/2024.
//

#include "StationImageProvider.h"
#include "utils/StringUtils.h"

#define MISSING_IMAGE ":/images/ImageMissing.jpg"
#define MISSING_RADIO ":/images/BlackRadio.svg"

StationImageProvider::StationImageProvider()

	: default_image_(MISSING_IMAGE),
	  default_radio_(MISSING_RADIO),
	  provider_manager_(std::make_unique<ImageProviderPriorityManager>())
{

}

void StationImageProvider::AddProvider(
	const std::shared_ptr<IImageProvider>& provider,
	int priority) const
{
  provider_manager_->AddProvider(provider, priority);
}

ImageData StationImageProvider::FetchStationImage(
	const Channel &channel) const
{
  for(const auto &entry : provider_manager_->Providers())
  {
	ImageData image = entry.provider->FetchStationImage(channel);
	if(!image.IsEmpty())
	{
	  return image;
	}
  }
  return MissingRadio();
}

ImageData StationImageProvider::FetchPrimaryImage(
	const Channel &channel,
	const NRSC5::ID3& id3) const
{
  for(const auto &[provider, priority] : provider_manager_->Providers())
  {
	ImageData image = provider->FetchPrimaryImage(channel, id3);

  	// If the image is not empty, return it
	if(!image.IsEmpty())
	{
	  return image;
	}

  }
  return MissingImage();
}

ImageData StationImageProvider::MissingRadio() const
{
	return {default_radio_, MISSING_RADIO, ImageData::Type::kMissing};
}

ImageData StationImageProvider::MissingImage() const
{
	return {default_image_, MISSING_IMAGE, ImageData::Type::kMissing};
}
