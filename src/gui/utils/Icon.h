//
// Created by TheDaChicken on 7/27/2025.
//

#ifndef ICON_H
#define ICON_H

#include <cmath>
#include <filesystem>
#include <fstream>

#include "gui/image_decoders/ImageBuffer.h"
#include "gui/image_decoders/SVGDecoder.h"
#include "gui/wrappers/Texture.h"
#include "gui/wrappers/TextureUploader.h"

namespace GUI
{
struct SVGImage
{
	explicit SVGImage(const std::shared_ptr<GPU::TextureUploader> &uploader)
		: uploader_(uploader),
		  texture(uploader->CreateTexture())
	{
	}

	FileLoadResult Load(const std::filesystem::path &fullPath)
	{
		path = fullPath;

		std::ifstream svgFile(path);
		if (!svgFile.is_open())
			return FileNotFound;

		return decoder.Decode(svgFile);
	}

	GPU::Texture &UpdateSize(const float height)
	{
		if (!decoder)
			return texture;

		const int h = std::ceil(height);
		if (texture.height != h)
		{
			ImageBuffer data;
			data.uri = path.string();
			data.type = FileType::SVG;
			data.height = h;

			decoder.Render(data);
			uploader_->LoadImage(texture, data);
		}
		return texture;
	}

	[[nodiscard]] GPU::Texture &GetTexture()
	{
		return texture;
	}

	SVGDecoder decoder;
	std::filesystem::path path;
	std::shared_ptr<GPU::TextureUploader> uploader_;
	GPU::Texture texture;
};
}

#endif //ICON_H
