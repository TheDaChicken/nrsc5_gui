//
// Created by TheDaChicken on 7/13/2025.
//

#ifndef IMAGE_ASSET_H
#define IMAGE_ASSET_H
#include <string>

#include "image/Image.h"
#include "backends/GlfwImpl.h"

namespace GUI
{
struct TextureAsset
{
	explicit TextureAsset(const Image &filePath);
	~TextureAsset();

	TextureAsset(const TextureAsset &) = delete;
	TextureAsset &operator=(const TextureAsset &) = delete;
	TextureAsset(TextureAsset &&) = default;

	ImVec2 size;
	ImTextureID texture;
};
}

#endif //IMAGE_ASSET_H
