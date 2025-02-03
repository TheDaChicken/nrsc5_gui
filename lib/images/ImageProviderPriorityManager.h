//
// Created by TheDaChicken on 8/24/2024.
//

#ifndef NRSC5_GUI_LIB_IMAGES_IMAGEPROVIDERPRIORITYMANAGER_H_
#define NRSC5_GUI_LIB_IMAGES_IMAGEPROVIDERPRIORITYMANAGER_H_

#include <memory>
#include <typeindex>
#include "images/providers/IImageProvider.h"

class ImageProviderPriorityManager
{
 public:
  struct ProviderEntry
  {
	std::shared_ptr<IImageProvider> provider;
	int priority;
  };

  void AddProvider(const std::shared_ptr<IImageProvider> &provider, int priority);

  [[nodiscard]] const std::vector<ProviderEntry>& Providers() const
  {
	return providers_;
  }

  template<typename T>
  std::shared_ptr<T> GetProvider() const
  {
	auto it = provider_map_.find(std::type_index(typeid(T)));
	if (it != provider_map_.end())
	{
	  return std::static_pointer_cast<T>(it->second);
	}
	return nullptr;
  }
 private:
  std::vector<ProviderEntry> providers_;
  std::unordered_map<std::type_index, std::shared_ptr<IImageProvider>> provider_map_;
};

#endif //NRSC5_GUI_LIB_IMAGES_IMAGEPROVIDERPRIORITYMANAGER_H_
