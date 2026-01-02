//
// Created by TheDaChicken on 9/7/2025.
//

#ifndef GPUCONTEXT_H
#define GPUCONTEXT_H

#include "SDLTexture.h"
#include "gui/platform/IGPUContext.h"
#include "gui/platform/ITexture.h"
#include "SDLTextureManager.h"
#include "SDLWindow.h"

namespace GUI
{
class SDLPlatformContext final : public IPlatformContext
{
	public:
		SDLPlatformContext();
		~SDLPlatformContext() override;

		bool OpenDevice();
		void SetDefaultTheme();

		const std::unique_ptr<ITexture> CreateTexture() override
		{
			return std::make_unique<SDLTexture>(uploader_);
		}

		std::unique_ptr<SDLWindow> CreateWindow()
		{
			return std::make_unique<SDLWindow>(gpu_device_);
		}

		SDL_GPUDevice* GetDevice()
		{
			return gpu_device_.get();
		}

	private:
		std::shared_ptr<SDL_GPUDevice> gpu_device_;
		std::shared_ptr<SDLTextureManager> uploader_;
};
} // namespace GPU

#endif //GPUCONTEXT_H
