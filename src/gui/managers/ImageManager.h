//
// Created by TheDaChicken on 8/25/2025.
//

#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <mutex>
#include <queue>

#include "images/ImageBuffer.h"
#include "gui/utils/Cache.h"
#include "gui/platform/IGPUContext.h"
#include "utils/ThreadPool.h"

namespace GUI
{
/**
 * A handle for holding queued textures.
 */
class TextureHandle
{
	public:
		TextureHandle() = default;
		explicit TextureHandle(std::shared_ptr<ITexture> texture)
			: texture(std::move(texture))
		{
		}

		[[nodiscard]] bool IsLoaded() const
		{
			return texture != nullptr && texture->IsAllocated();
		}

		[[nodiscard]] std::shared_ptr<ITexture> Get() const
		{
			return texture;
		}

	private:
		std::shared_ptr<ITexture> texture;
};

/**
 * An easy-to-use manager for queueing up images
 * to be loaded later on render thread.
 */
class ImageManager
{
	public:
		struct LoadRequest
		{
			ImageBuffer image;
			std::string key;
		};

		explicit ImageManager(const std::shared_ptr<IPlatformContext> &uploader)
			: uploader_(uploader), image_cache_(100)
		{
		}

		~ImageManager() = default;

		ImageManager(const ImageManager &) = delete;
		ImageManager &operator=(const ImageManager &) = delete;

		std::pair<bool, TextureHandle> CreateImage(
			const std::string &key);

		bool ContainsImage(const std::string &key) const
		{
			return image_cache_.Contains(key);
		}

		void QueueImage(const std::string &key, const ImageBuffer &image);
		void Process();

	private:
		const std::shared_ptr<IPlatformContext> uploader_;

		std::mutex queue_mutex_;
		std::queue<LoadRequest> load_queue_;
		Cache<std::string, ITexture> image_cache_;
};
}

#endif //IMAGEMANAGER_H
