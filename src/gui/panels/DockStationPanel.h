//
// Created by TheDaChicken on 7/30/2025.
//

#ifndef DOCKSTATIONPANEL_H
#define DOCKSTATIONPANEL_H

#include "DockInputPanel.h"
#include "gui/managers/ThemeManager.h"
#include "gui/view/IView.h"

class DockStationPanel final : public IView
{
	public:
		explicit DockStationPanel(const std::shared_ptr<UISession> &session);

		void Render(RenderContext &context) override;

	private:
		static std::shared_ptr<GUI::ITexture> FirstLoadedTexture(
			const std::initializer_list<std::reference_wrapper<const GUI::TextureHandle> > images,
			const std::shared_ptr<GUI::ITexture> &fallback)
		{
			for (const auto &img : images)
			{
				if (img.get().IsLoaded())
					return img.get().Get();
			}
			return fallback;
		}

		// static void RenderStationHeader(
		// 	const Theme &theme, const HybridState &state);
		// static void RenderStationDetails(
		// 	const Theme &theme,
		// 	const HybridState &state,
		// 	const NRSC5::Program &program);
		//
		// static void RenderRadioInfo(const Theme &theme, const HybridState &state);
		//
		// static void RenderImage(
		// 	const std::shared_ptr<GUI::ITexture> &texture, float height);
		// static void RenderStationImage(const Theme &theme, const HybridState &state);
		// static void RenderPrimaryImage(const Theme &theme, const HybridState &state);
		// static void RenderID3(const Theme &theme, const NRSC5::ID3 &id3);
		//
		// static void RenderProgramList(
		// 	const Theme &theme, const HybridState &state);
		static void RenderProgramNumbers(
			const std::map<unsigned int, NRSC5::Program> &programs, unsigned int currId);

		void RenderControls(const Theme &theme) const;

		std::shared_ptr<UISession> session_;
};

#endif //DOCKSTATIONPANEL_H
