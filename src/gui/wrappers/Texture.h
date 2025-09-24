//
// Created by TheDaChicken on 7/13/2025.
//

#ifndef IMAGE_ASSET_H
#define IMAGE_ASSET_H

#include <memory>
#include <SDL3/SDL_gpu.h>

#include "utils/Expected.hpp"

namespace GPU
{
struct Texture
{
	explicit Texture(std::shared_ptr<SDL_GPUDevice> device);
	~Texture();

	Texture(const Texture &) = delete; // no copy
	Texture &operator=(const Texture &) = delete; // no copy-assign
	Texture(Texture &&other) noexcept;
	Texture &operator=(Texture &&other) noexcept;

	bool CreateTexture(int width, int height);
	void Release();

	void UploadTexture(SDL_GPUTransferBuffer *transfer_buffer) const;

	std::shared_ptr<SDL_GPUDevice> gpu_device_;
	std::unique_ptr<SDL_GPUTexture, std::function<void(SDL_GPUTexture *)> > ptr;
	int width;
	int height;
};
}

#endif //IMAGE_ASSET_H
