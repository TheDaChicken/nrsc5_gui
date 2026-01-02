//
// Created by TheDaChicken on 9/7/2025.
//

#include "SDLPlatformContext.h"
#include "gui/platform/sdl/SDLTextureManager.h"
#include "utils/Log.h"

GUI::SDLPlatformContext::SDLPlatformContext()
{
}

GUI::SDLPlatformContext::~SDLPlatformContext()
{
	SDL_WaitForGPUIdle(gpu_device_.get());
}

bool GUI::SDLPlatformContext::OpenDevice()
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
	auto new_uploader = std::make_shared<SDLTextureManager>(new_gpu_device);
	if (!new_uploader)
	{
		Logger::Log(err, "Failed to create texture uploader");
		return false;
	}

	gpu_device_ = std::move(new_gpu_device);
	uploader_ = std::move(new_uploader);
	return true;
}

void GUI::SDLPlatformContext::SetDefaultTheme()
{
	SDL_SetPreferredSystemTheme(SDL_SYSTEM_THEME_LIGHT);
}
