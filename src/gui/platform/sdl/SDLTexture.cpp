//
// Created by TheDaChicken on 7/13/2025.
//

#include "SDLTexture.h"

#include <SDL3/SDL_render.h>

#include "Application.h"

GUI::SDLTexture::SDLTexture(std::shared_ptr<SDLTextureManager> manager)
	: manager_(std::move(manager)),
	  ptr_(nullptr, nullptr),
	  height_(0),
	  width_(0)
{
}

GUI::SDLTexture::SDLTexture(SDLTexture &&other) noexcept
	: manager_(std::move(other.manager_)),
	  ptr_(std::move(other.ptr_)),
	  height_(other.height_),
	  width_(other.width_)
{
	other.ptr_ = nullptr;
	other.width_ = 0;
	other.height_ = 0;
}

GUI::SDLTexture &GUI::SDLTexture::operator=(SDLTexture &&other)
{
	if (this != &other)
	{
		if (manager_ != other.manager_)
			throw std::runtime_error("Cannot move Textures from different managers");

		ptr_ = std::move(other.ptr_);
		width_ = other.width_;
		height_ = other.height_;

		other.ptr_ = nullptr;
		other.width_ = 0;
		other.height_ = 0;
	}
	return *this;
}

bool GUI::SDLTexture::LoadImage(const ImageBuffer &image)
{
	assert(image.color_type == GUI::ImageBuffer::ColorType::RGBA);

	const int pitch = image.width * 4;
	const uint32_t upload_size = image.width * pitch + 1204;

	Logger::Log(debug,
	            "SDLTexture: Loading image {} ({}x{})",
	            image.uri,
	            image.width,
	            image.height);

	if (!AllocateTexture(image.width, image.height))
		return false;

	manager_->ResizeGpuTransfer(upload_size);

	{
		const auto buffer = manager_->GetGPUBuffer();
		void *texture_ptr = buffer.get();

		for (int y = 0; y < image.height; y++)
			memcpy(reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(texture_ptr) + y * pitch),
			       image.data.data() + y * pitch,
			       pitch);
	}

	TextureTransfer transfer(ptr_);
	transfer.offset = 0;
	transfer.width = image.width;
	transfer.height = image.height;

	manager_->PushToGpuTexture(transfer);

	Logger::Log(debug,
	            "SDLTexture: Successfully loaded image {} ({}x{})",
	            image.uri,
	            image.width,
	            image.height);
	return true;
}

bool GUI::SDLTexture::AllocateTexture(const int width, const int height)
{
	if (ptr_ != nullptr &&
		this->GetWidth() == width &&
		this->GetHeight() == height)
		return true;

	auto new_texture = manager_->CreateTexture(width, height);
	if (!new_texture)
		return false;

	ptr_ = std::move(new_texture);
	width_ = width;
	height_ = height;
	return true;
}

