//
// Created by TheDaChicken on 9/15/2025.
//

#ifndef DOCKTUNEPANEL_H
#define DOCKTUNEPANEL_H

#include "gui/managers/ThemeManager.h"
#include "hybrid/HybridInput.h"

class DockTunePanel
{
	public:
		explicit DockTunePanel(HybridInput &input);

		bool Render(const Theme &theme);

	private:
		void UpdateState(Band::Type selected);

		void RenderBand(const std::string &label, Band::Type band, float height);
		void RenderBands(float height);
		void RenderButtons(float height);
		void RenderFreq(float height) const;
		bool RenderTuneButton(float height);
		static float GetRowHeight(float avail_height, float rows);

		struct FreqState
		{
			uint32_t scale;
			int64_t freq_min;
			int64_t freq_max;

			double freq_scale_den;
			int64_t selected_freq;

			int decimal_places;
		} state_;

		HybridInput &input_;
		Band::Type selected_band_ = Band::FM;
};

#endif //DOCKTUNEPANEL_H
