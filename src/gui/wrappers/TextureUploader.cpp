//
// Created by TheDaChicken on 9/7/2025.
//

#include "TextureUploader.h"

#include <cassert>

#include "utils/Log.h"

GPU::TextureUploader::TextureUploader(const std::shared_ptr<SDL_GPUDevice> &device)
	: device_(device),
	  transfer_buffer_len(0),
	  transfer_buffer_(nullptr)
{
}

GPU::TextureUploader::~TextureUploader()
{
	if (transfer_buffer_)
		SDL_ReleaseGPUTransferBuffer(device_.get(), transfer_buffer_);
}

void GPU::TextureUploader::LoadImage(Texture &texture, const GUI::ImageBuffer &image)
{
	assert(image.color_type == GUI::ImageBuffer::ColorType::RGBA);

	const int pitch = image.width * 4;

	Logger::Log(debug,
	            "TextureUploader: Loading image {} ({}x{})",
	            image.uri,
	            image.width,
	            image.height);

	if (!texture.CreateTexture(image.width, image.height))
	{
		Logger::Log(err, "TextureUploader: Failed to create texture for image {}", image.uri);
		return;
	}

	const uint32_t upload_size = image.width * pitch + 1204;

	if (transfer_buffer_len < upload_size)
	{
		SDL_ReleaseGPUTransferBuffer(device_.get(), transfer_buffer_);

		// Create transfer buffer
		SDL_GPUTransferBufferCreateInfo transferbuffer_info = {};
		transferbuffer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
		transferbuffer_info.size = upload_size;

		transfer_buffer_ = SDL_CreateGPUTransferBuffer(device_.get(), &transferbuffer_info);
		assert(transfer_buffer_ != nullptr);

		transfer_buffer_len = upload_size;
	}

	{
		void *texture_ptr = SDL_MapGPUTransferBuffer(device_.get(), transfer_buffer_, true);
		for (int y = 0; y < image.height; y++)
			memcpy(reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(texture_ptr) + y * pitch),
			       image.data.data() + y * pitch,
			       pitch);
		SDL_UnmapGPUTransferBuffer(device_.get(), transfer_buffer_);
	}

	texture.UploadTexture(transfer_buffer_);

	Logger::Log(debug,
	            "TextureUploader: Successfully loaded image {} ({}x{})",
	            image.uri,
	            image.width,
	            image.height);
}
