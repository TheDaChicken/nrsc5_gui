//
// Created by TheDaChicken on 7/30/2025.
//

#ifndef DOCKSTATIONPANEL_H
#define DOCKSTATIONPANEL_H

#include "gui/managers/ThemeManager.h"
#include "hybrid/HybridInput.h"

class DockStationPanel
{
	public:
		explicit DockStationPanel(
			HybridInput &input,
			const std::weak_ptr<HybridSession> &session);

		void Render(const Theme &theme);

	private:
		static std::shared_ptr<GPU::Texture> FirstLoadedTexture(
			const std::initializer_list<std::reference_wrapper<const TextureHandle> > images,
			const std::shared_ptr<GPU::Texture> &fallback)
		{
			for (const auto &img : images)
			{
				if (img.get().IsLoaded())
					return img.get().get();
			}
			return fallback;
		}

		void RenderStationHeader(const Theme &theme);
		void RenderRadioInfo(const Theme &theme) const;

		void RenderStationLogo(
			const Theme &theme) const;
		void RenderStationDetails(
			const Theme &theme) const;
		void RenderProgramList(
			const Theme &theme);
		void RenderProgramNumbers(
			const std::map<unsigned int, ProgramState> &programs) const;

		void RenderPrimaryImage(const Theme &theme) const;
		static void RenderID3(const Theme &theme, const NRSC5::ID3 &id3);
		void RenderControls(const Theme &theme) const;

		const HybridInput &input_;
		const std::weak_ptr<HybridSession> session_;

		HybridState frame_state_;
		ProgramState frame_program_;
		std::map<unsigned int, ProgramState> frame_programs_;
};

#endif //DOCKSTATIONPANEL_H
