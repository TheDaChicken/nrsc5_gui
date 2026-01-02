//
// Created by TheDaChicken on 7/13/2025.
//

#ifndef IMAGE_ASSET_H
#define IMAGE_ASSET_H

#include <memory>
#include <SDL3/SDL_gpu.h>

#include "gui/platform/ITexture.h"
#include "SDLTextureManager.h"

namespace GUI
{
class SDLTexture final : public ITexture
{
	public:
		explicit SDLTexture(std::shared_ptr<SDLTextureManager> manager);
		~SDLTexture() override = default;

		SDLTexture(const SDLTexture &) = delete; // no copy
		SDLTexture &operator=(const SDLTexture &) = delete; // no copy-assign
		SDLTexture(SDLTexture &&other) noexcept;
		SDLTexture &operator=(SDLTexture &&other);

		bool LoadImage(const ImageBuffer &image) override;
		bool AllocateTexture(int width, int height);

		[[nodiscard]] int GetWidth() const override
		{
			return width_;
		}

		[[nodiscard]] int GetHeight() const override
		{
			return height_;
		}

		[[nodiscard]] uintptr_t GetPtr() const override
		{
			return reinterpret_cast<intptr_t>(ptr_.get());
		}

		[[nodiscard]] bool IsAllocated() const override
		{
			return ptr_ != nullptr;
		}

	private:
		const std::shared_ptr<SDLTextureManager> manager_;
		SDLTEXTURE_PTR ptr_;

		int height_;
		int width_;
};
}

#endif //IMAGE_ASSET_H
