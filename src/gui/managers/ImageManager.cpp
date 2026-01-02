//
// Created by TheDaChicken on 8/25/2025.
//

#include "ImageManager.h"

#include <future>

#include "images/ImageDecoder.h"
#include "utils/Log.h"

std::pair<bool, GUI::TextureHandle> GUI::ImageManager::CreateImage(const std::string &key)
{
	auto p = uploader_->CreateTexture();
	if (!p)
		return {false, {}};

	const auto &[success, texture] = image_cache_.TryInsert(
		key,
		1,
		std::move(p));
	return std::make_pair(success, TextureHandle(texture));
}

void GUI::ImageManager::QueueImage(const std::string &key, const ImageBuffer &image)
{
	std::scoped_lock lock(queue_mutex_);
	load_queue_.emplace(LoadRequest{image, key});
}

void GUI::ImageManager::Process()
{
	std::scoped_lock lock(queue_mutex_);

	while (!load_queue_.empty())
	{
		const auto req = std::move(load_queue_.front());
		load_queue_.pop();

		const auto &cache = image_cache_.Get(req.key);
		if (!cache)
		{
			Logger::Log(warn, "ImageManager: No cache found for key: {}", req.key);
			continue;
		}

		Logger::Log(trace, "ImageManager: Loading image for key: {}", req.key);

		cache->LoadImage(req.image);
	}
}
