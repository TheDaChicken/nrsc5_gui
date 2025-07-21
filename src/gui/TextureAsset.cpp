//
// Created by TheDaChicken on 7/13/2025.
//

#include "TextureAsset.h"

GUI::TextureAsset::TextureAsset(const Image &image)
{
	size = {static_cast<float>(image.width), static_cast<float>(image.height)};
	texture = GlfwImpl::LoadTexture(image);
}

GUI::TextureAsset::~TextureAsset()
{
	GlfwImpl::UnloadTexture(texture);
}

