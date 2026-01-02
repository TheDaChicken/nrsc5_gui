//
// Created by TheDaChicken on 9/7/2025.
//

#ifndef TEXTUREUPLOADER_H
#define TEXTUREUPLOADER_H

#include <functional>
#include <memory>
#include <SDL3/SDL_gpu.h>

namespace GUI
{
using SDLTEXTURE_PTR = std::unique_ptr<
	SDL_GPUTexture, std::function<void(SDL_GPUTexture *)> >;

using SDLGPUTRANSFER_PTR = std::unique_ptr<
	SDL_GPUTransferBuffer, std::function<void(SDL_GPUTransferBuffer *)> >;

using SDLGPUBUFFER = std::unique_ptr<void, std::function<void(void *pointer)> >;

struct TextureTransfer
{
	explicit TextureTransfer(const SDLTEXTURE_PTR &ptr_)
		: ptr(ptr_)
	{
	}

	int offset;
	int width;
	int height;
	const SDLTEXTURE_PTR &ptr;
};

class SDLTextureManager final
		: public std::enable_shared_from_this<SDLTextureManager>
{
	public:
		explicit SDLTextureManager(const std::shared_ptr<SDL_GPUDevice> &device);

		SDLTEXTURE_PTR CreateTexture(int width, int height) const;
		SDLGPUBUFFER GetGPUBuffer() const;

		void ResizeGpuTransfer(uint32_t size);
		void PushToGpuTexture(const TextureTransfer &transfer) const;

	private:
		std::shared_ptr<SDL_GPUDevice> device_;

		std::size_t transfer_buffer_len;
		SDLGPUTRANSFER_PTR transfer_buffer_;
};
} // namespace GUI
#endif //TEXTUREUPLOADER_H
