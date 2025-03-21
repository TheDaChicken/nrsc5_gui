//
// Created by TheDaChicken on 7/29/2024.
//

#ifndef NRSC5_GUI_SRC_PAGES_RADIODASHBOARD_H_
#define NRSC5_GUI_SRC_PAGES_RADIODASHBOARD_H_

#include <controllers/RadioController.h>

#include "RadioChannel.h"
#include "LoadingPage.h"

#include "views/RadioMainView.h"
#include "views/RadioControlPanelView.h"
#include "widgets/dualview/DualViewWidget.h"

/**
 * Contains full controllability of all tuner pages.
 */
class RadioPage : public DualViewWidget
{
		Q_OBJECT

	public:
		explicit RadioPage(QWidget *parent = nullptr);
		~RadioPage() override;

		void UpdateTunerStatus(const TunerAction& action, const UTILS::StatusCodes& state) const;

		void SwitchToMain() const;
		void ShowControlPanel() const;

		[[nodiscard]] RadioMainView *RadioMain() const
		{
			return main_view;
		}

		[[nodiscard]] RadioControlPanelView *ControlPanel() const
		{
			return control_panel_view;
		}

		[[nodiscard]] LoadingPage *StatusPage() const
		{
			return dynamic_cast<LoadingPage *>(status_view->GetMainWidget());
		}

	private:
		[[nodiscard]] QString ActionString(const TunerAction& action) const;

		RadioMainView *main_view;
		RadioControlPanelView *control_panel_view;
		DualViewContainer *status_view;
};

#endif //NRSC5_GUI_SRC_PAGES_RADIODASHBOARD_H_
