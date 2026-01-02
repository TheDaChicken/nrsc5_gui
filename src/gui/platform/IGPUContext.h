//
// Created by TheDaChicken on 11/24/2025.
//

#ifndef NRSC5_GUI_IGPUCONTEXT_H
#define NRSC5_GUI_IGPUCONTEXT_H

#include <memory>

#include "ITexture.h"

namespace GUI
{
class IPlatformContext
{
	public:
		virtual ~IPlatformContext() = default;
		virtual bool OpenDevice() = 0;

		virtual void SetDefaultTheme() = 0;
		virtual const std::unique_ptr<ITexture> CreateTexture() = 0;
};
}

#endif //NRSC5_GUI_IGPUCONTEXT_H
