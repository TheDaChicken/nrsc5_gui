//
// Created by TheDaChicken on 8/25/2025.
//

#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <imgui.h>
#include <mutex>
#include <queue>

#include "gui/image_decoders/ImageBuffer.h"
#include "gui/utils/Cache.h"
#include "gui/wrappers/Texture.h"
#include "gui/wrappers/TextureUploader.h"
#include "utils/ThreadPool.h"

class TextureHandle
{
	public:
		TextureHandle() = default;
		explicit TextureHandle(std::shared_ptr<GPU::Texture> texture)
			: texture(std::move(texture))
		{

		}

		[[nodiscard]] int Width() const
		{
			if (!texture)
				return 0;
			return texture->width;
		}

		[[nodiscard]] int Height() const
		{
			if (!texture)
				return 0;
			return texture->height;
		}

		bool IsLoaded() const
		{
			return texture != nullptr && texture->ptr != nullptr;
		}

		[[nodiscard]] std::shared_ptr<GPU::Texture> get() const
		{
			return texture;
		}

	private:
		std::shared_ptr<GPU::Texture> texture;
};

class ImageManager
{
	public:
		struct LoadRequest
		{
			GUI::ImageBuffer image;
			std::string key;
		};

		explicit ImageManager(const std::shared_ptr<GPU::TextureUploader> &uploader)
			: image_cache_(100), uploader_(uploader)
		{
		}

		~ImageManager() = default;

		ImageManager(const ImageManager &) = delete;
		ImageManager &operator=(const ImageManager &) = delete;

		std::pair<bool, TextureHandle > CreateImageCache(
			const std::string &key);

		bool ContainsImage(const std::string &key) const
		{
			return image_cache_.Contains(key);
		}

		void QueueImage(const std::string &key, const GUI::ImageBuffer &image);
		void Process();

	private:
		std::mutex queue_mutex_;
		std::queue<LoadRequest> load_queue_;
		Cache<std::string, GPU::Texture> image_cache_;

		std::shared_ptr<GPU::TextureUploader> uploader_;
};

#endif //IMAGEMANAGER_H
