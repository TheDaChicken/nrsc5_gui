//
// Created by TheDaChicken on 11/20/2025.
//

#ifndef NRSC5_GUI_ITEXTURE_H
#define NRSC5_GUI_ITEXTURE_H

#include "images/ImageBuffer.h"

namespace GUI
{
class ITexture
{
	public:
		virtual ~ITexture() = default;

		[[nodiscard]] virtual int GetWidth() const = 0;
		[[nodiscard]] virtual int GetHeight() const = 0;

		[[nodiscard]] virtual uintptr_t GetPtr() const = 0;
		[[nodiscard]] virtual bool IsAllocated() const = 0;

		[[nodiscard]] virtual bool LoadImage(const ImageBuffer &image) = 0;
};
}

#endif //NRSC5_GUI_ITEXTURE_H
