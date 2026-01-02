//
// Created by TheDaChicken on 9/15/2025.
//

#ifndef DOCKTUNEPANEL_H
#define DOCKTUNEPANEL_H

#include "gui/managers/ThemeManager.h"
#include "gui/UISession.h"

class DockTunePanel
{
	public:
		explicit DockTunePanel(const std::shared_ptr<UISession>& input);

		bool Render(const Theme &theme);

	private:
		void UpdateState(Band::Type selected);

		void RenderBand(const std::string &label, Band::Type band);
		void RenderBands();
		void RenderButtons();
		void RenderFreq() const;
		bool RenderTuneButton();

		struct FreqState
		{
			uint32_t scale;
			int64_t freq_min;
			int64_t freq_max;

			double freq_scale_den;
			int64_t selected_freq;

			int decimal_places;
		} state_;

		std::shared_ptr<UISession> input_;
		Band::Type selected_band_ = Band::FM;
};

#endif //DOCKTUNEPANEL_H
