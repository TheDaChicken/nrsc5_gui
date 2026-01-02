//
// Created by TheDaChicken on 7/27/2025.
//

#ifndef ICON_H
#define ICON_H

#include <cmath>
#include <filesystem>
#include <fstream>

#include "images/ImageBuffer.h"
#include "images/SVGDecoder.h"
#include "gui/platform/IGPUContext.h"

namespace GUI
{
class SVGImage
{
	public:
		explicit SVGImage(const std::shared_ptr<IPlatformContext> &context)
			: texture(context->CreateTexture())
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

		std::unique_ptr<ITexture> &UpdateSize(const float height)
		{
			if (!decoder)
				return texture;

			const int h = std::ceil(height);
			if (texture->GetHeight() != h)
			{
				ImageBuffer data;
				data.uri = path.string();
				data.type = FileType::SVG;
				data.height = h;

				decoder.Render(data);
				texture->LoadImage(data);
			}
			return texture;
		}

		[[nodiscard]] std::unique_ptr<GUI::ITexture> &GetTexture()
		{
			return texture;
		}

	private:
		SVGDecoder decoder;
		std::filesystem::path path;
		std::unique_ptr<ITexture> texture;
};
}

#endif //ICON_H
