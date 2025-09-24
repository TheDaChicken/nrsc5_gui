//
// Created by TheDaChicken on 8/25/2025.
//

#include "ImageManager.h"

#include <future>

#include "gui/image_decoders/ImageDecoder.h"
#include "utils/Log.h"

std::pair<bool, TextureHandle > ImageManager::CreateImageCache(const std::string &key)
{
	const auto &[success, cache] = image_cache_.TryInsert(key, 1, uploader_->CreateTexture());
	return std::make_pair(success, TextureHandle(cache));
}

void ImageManager::QueueImage(const std::string &key, const GUI::ImageBuffer &image)
{
	std::lock_guard lock(queue_mutex_);
	load_queue_.emplace(LoadRequest{image, key});
}

void ImageManager::Process()
{
	std::lock_guard lock(queue_mutex_);

	while (!load_queue_.empty())
	{
		const auto &req = load_queue_.front();
		const auto &cache = image_cache_.Get(req.key);
		if (!cache)
		{
			Logger::Log(warn, "ImageManager: No cache found for key: {}", req.key);
			load_queue_.pop();
			continue;
		}

		Logger::Log(trace, "ImageManager: Loading image for key: {}", req.key);

		uploader_->LoadImage(*cache, req.image);

		load_queue_.pop();
	}
}
