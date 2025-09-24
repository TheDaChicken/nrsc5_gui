//
// Created by TheDaChicken on 9/7/2025.
//

#ifndef GPUCONTEXT_H
#define GPUCONTEXT_H
#include "TextureUploader.h"

namespace GPU
{
class GPUContext
{
	public:
		GPUContext();
		~GPUContext();

		bool OpenDevice();

		const std::shared_ptr<TextureUploader> &GetUploader()
		{
			return uploader_;
		}

		SDL_GPUDevice* GetDevice()
		{
			return gpu_device_.get();
		}

	private:
		std::shared_ptr<SDL_GPUDevice> gpu_device_;
		std::shared_ptr<TextureUploader> uploader_;
};
} // namespace GPU

#endif //GPUCONTEXT_H
