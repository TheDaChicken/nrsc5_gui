//
// Created by TheDaChicken on 9/7/2025.
//

#include "SDLTextureManager.h"

#include <cassert>

#include "utils/Log.h"

GUI::SDLTextureManager::SDLTextureManager(const std::shared_ptr<SDL_GPUDevice> &device)
	: device_(device),
	  transfer_buffer_len(0)
{
}

GUI::SDLTEXTURE_PTR GUI::SDLTextureManager::CreateTexture(
	const int width,
	const int height) const
{
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

	const auto &device = device_;

	auto new_texture = SDLTEXTURE_PTR(
		SDL_CreateGPUTexture(device.get(), &texture_info),
		[device](SDL_GPUTexture *tex)
		{
			SDL_ReleaseGPUTexture(device.get(), tex);
		});
	if (!new_texture)
	{
		Logger::Log(err,
		            "SDLTexture: "
		            "Failed to create texture: {}",
		            SDL_GetError());
		return {};
	}
	return std::move(new_texture);
}

GUI::SDLGPUBUFFER GUI::SDLTextureManager::GetGPUBuffer() const
{
	return SDLGPUBUFFER(
		SDL_MapGPUTransferBuffer(
			device_.get(),
			transfer_buffer_.get(),
			true),
		[this](void *pointer)
		{
			SDL_UnmapGPUTransferBuffer(
				device_.get(),
				transfer_buffer_.get());
		});
}

void GUI::SDLTextureManager::ResizeGpuTransfer(const uint32_t size)
{
	if (size < transfer_buffer_len)
		return;

	transfer_buffer_.reset();

	// Create transfer buffer
	SDL_GPUTransferBufferCreateInfo transferbuffer_info = {};
	transferbuffer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	transferbuffer_info.size = size;

	transfer_buffer_ = SDLGPUTRANSFER_PTR(
		SDL_CreateGPUTransferBuffer(
			device_.get(),
			&transferbuffer_info),
		[this](SDL_GPUTransferBuffer *buffer)
		{
			SDL_ReleaseGPUTransferBuffer(device_.get(), buffer);
		});
	assert(transfer_buffer_ != nullptr);

	transfer_buffer_len = size;
}

void GUI::SDLTextureManager::PushToGpuTexture(const TextureTransfer &transfer) const
{
	SDL_GPUTextureTransferInfo transfer_info = {};
	transfer_info.offset = 0;
	transfer_info.transfer_buffer = transfer_buffer_.get();

	SDL_GPUTextureRegion texture_region = {};
	texture_region.texture = transfer.ptr.get();
	texture_region.x = static_cast<Uint32>(0);
	texture_region.y = static_cast<Uint32>(0);
	texture_region.w = static_cast<Uint32>(transfer.width);
	texture_region.h = static_cast<Uint32>(transfer.height);
	texture_region.d = 1;

	// Upload
	SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device_.get());
	SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(cmd);
	SDL_UploadToGPUTexture(copy_pass, &transfer_info, &texture_region, false);
	SDL_EndGPUCopyPass(copy_pass);
	SDL_SubmitGPUCommandBuffer(cmd);
}
