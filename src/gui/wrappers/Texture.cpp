//
// Created by TheDaChicken on 7/13/2025.
//

#include "Texture.h"

#include <SDL3/SDL_render.h>

#include "Application.h"

GPU::Texture::Texture(std::shared_ptr<SDL_GPUDevice> device)
	: gpu_device_(std::move(device)),
	  ptr(nullptr,
	      [](SDL_GPUTexture *)
	      {
	      }),
	  width(0),
	  height(0)
{
}

GPU::Texture::~Texture()
{
	Release();
}

bool GPU::Texture::CreateTexture(const int width, const int height)
{
	if (this->width == width && this->height == height && ptr != nullptr)
		return true;

	// Create texture
	SDL_GPUTextureCreateInfo texture_info = {};
	texture_info.type = SDL_GPU_TEXTURETYPE_2D;
	texture_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
	texture_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
	texture_info.width = width;
	texture_info.height = height;
	texture_info.layer_count_or_depth = 1;
	texture_info.num_levels = 1;
	texture_info.sample_count = SDL_GPU_SAMPLECOUNT_1;

	auto newTexture = std::unique_ptr<SDL_GPUTexture, std::function<void(SDL_GPUTexture *)> >(
		SDL_CreateGPUTexture(gpu_device_.get(), &texture_info),
		[this](SDL_GPUTexture *tex)
		{
			if (!gpu_device_)
			{
				Logger::Log(err, "Texture: No valid gpu device to release texture");
				return;
			}
			SDL_ReleaseGPUTexture(gpu_device_.get(), tex);
		});

	if (newTexture != nullptr)
	{
		this->ptr = std::move(newTexture);
		this->width = width;
		this->height = height;
		return true;
	}

	Logger::Log(err, "Texture: Failed to create texture: {}", SDL_GetError());
	return false;
}

void GPU::Texture::Release()
{
	if (!ptr)
		return;

	ptr.reset();
}

void GPU::Texture::UploadTexture(SDL_GPUTransferBuffer *transfer_buffer) const
{
	SDL_GPUTextureTransferInfo transfer_info = {};
	transfer_info.offset = 0;
	transfer_info.transfer_buffer = transfer_buffer;

	SDL_GPUTextureRegion texture_region = {};
	texture_region.texture = ptr.get();
	texture_region.x = static_cast<Uint32>(0);
	texture_region.y = static_cast<Uint32>(0);
	texture_region.w = static_cast<Uint32>(width);
	texture_region.h = static_cast<Uint32>(height);
	texture_region.d = 1;

	// Upload
	{
		SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(gpu_device_.get());
		SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(cmd);
		SDL_UploadToGPUTexture(copy_pass, &transfer_info, &texture_region, false);
		SDL_EndGPUCopyPass(copy_pass);
		SDL_SubmitGPUCommandBuffer(cmd);
	}
}

GPU::Texture::Texture(Texture &&other) noexcept
	: gpu_device_(std::move(other.gpu_device_)),
	  ptr(std::move(other.ptr)),
	  width(other.width),
	  height(other.height)
{
	other.gpu_device_ = nullptr;
	other.ptr = nullptr;
	other.width = 0;
	other.height = 0;
}

GPU::Texture &GPU::Texture::operator=(Texture &&other) noexcept
{
	if (this != &other)
	{
		gpu_device_ = std::move(other.gpu_device_);
		ptr = std::move(other.ptr);
		width = other.width;
		height = other.height;

		other.ptr = nullptr;
		other.width = 0;
		other.height = 0;
	}
	return *this;
}

