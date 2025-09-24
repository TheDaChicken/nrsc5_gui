//
// Created by TheDaChicken on 9/7/2025.
//

#include "GPUContext.h"

#include "utils/Log.h"

GPU::GPUContext::GPUContext()
	: gpu_device_(nullptr, SDL_DestroyGPUDevice)
{
}

GPU::GPUContext::~GPUContext()
{
	SDL_WaitForGPUIdle(gpu_device_.get());
}

bool GPU::GPUContext::OpenDevice()
{
	auto new_gpu_device = std::shared_ptr<SDL_GPUDevice>(
		SDL_CreateGPUDevice(
			SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL |
			SDL_GPU_SHADERFORMAT_METALLIB,
			true,
			nullptr),
		SDL_DestroyGPUDevice);
	if (!new_gpu_device)
	{
		Logger::Log(err, "Failed to create GPU device {}", SDL_GetError());
		return false;
	}
	auto new_uploader = std::make_shared<TextureUploader>(new_gpu_device);
	if (!new_uploader)
	{
		Logger::Log(err, "Failed to create texture uploader");
		return false;
	}

	gpu_device_ = std::move(new_gpu_device);
	uploader_ = std::move(new_uploader);

	return true;
}
