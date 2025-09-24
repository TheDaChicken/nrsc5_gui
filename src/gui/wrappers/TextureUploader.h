//
// Created by TheDaChicken on 9/7/2025.
//

#ifndef TEXTUREUPLOADER_H
#define TEXTUREUPLOADER_H

#include <memory>
#include <SDL3/SDL_gpu.h>

#include "Texture.h"
#include "gui/image_decoders/ImageBuffer.h"

namespace GPU
{
class TextureUploader
{
	public:
		explicit TextureUploader(const std::shared_ptr<SDL_GPUDevice> &device);
		~TextureUploader();

		Texture CreateTexture()
		{
			return Texture(device_);
		}
		void LoadImage(Texture &texture, const GUI::ImageBuffer &image);

	private:
		std::shared_ptr<SDL_GPUDevice> device_;

		std::size_t transfer_buffer_len;
		SDL_GPUTransferBuffer *transfer_buffer_;
};

} // namespace GUI
#endif //TEXTUREUPLOADER_H
