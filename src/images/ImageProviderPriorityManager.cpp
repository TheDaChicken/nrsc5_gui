//
// Created by TheDaChicken on 8/24/2024.
//

#include "ImageProviderPriorityManager.h"

void ImageProviderPriorityManager::AddProvider(
	const std::shared_ptr<IImageProvider> &provider,
	int priority)
{
	providers_.emplace_back(ProviderEntry{provider, priority});
	// Sort providers by priority (highest priority first)
	std::sort(providers_.begin(),
	          providers_.end(),
	          [](const ProviderEntry &a, const ProviderEntry &b)
	          {
		          return a.priority > b.priority;
	          });
}
