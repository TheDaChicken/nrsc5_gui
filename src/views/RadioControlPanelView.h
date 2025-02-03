//
// Created by TheDaChicken on 8/15/2024.
//

#ifndef NRSC5_GUI_SRC_VIEW_RADIOCONTROLPANELVIEW_H_
#define NRSC5_GUI_SRC_VIEW_RADIOCONTROLPANELVIEW_H_

#include <QListWidgetItem>
#include <QSortFilterProxyModel>
#include <QStackedWidget>

#include "widgets/dualview/DualViewContainer.h"
#include "widgets/navigation/NavigationWidget.h"
#include "widgets/favorites/FavoritesList.h"
#include "widgets/StationFramePanel.h"
#include "widgets/LineHeader.h"
#include "widgets/sdr/TuneWidget.h"

#include "display/StationInfoPanel.h"

class RadioControlPanelView : public DualViewContainer
{
	public:
		explicit RadioControlPanelView();

		[[nodiscard]] NavigationWidget *GetHeader() const
		{
			return dynamic_cast<NavigationWidget *>(GetHeaderWidget());
		}

		[[nodiscard]] StationInfoPanel *GetStationInfoPanel() const
		{
			return frame_panel_->GetStationInfoPanel();
		}

		[[nodiscard]] QListView *GetRadioPanel() const
		{
			assert(radio_panel_);
			return radio_panel_;
		}

		[[nodiscard]] FavoritesList *GetFavoritesTree() const
		{
			assert(favorites_tree_);
			return favorites_tree_;
		}

		[[nodiscard]] TuneWidget *GetTuneWidget() const
		{
			assert(tune_widget_);
			return tune_widget_;
		}

		[[nodiscard]] LineHeader *GetFavoritesHeader() const
		{
			assert(favorites_header_);
			return favorites_header_;
		}

	private slots:
		void OnRadioPanelClicked(const QModelIndex &index) const;

	private:
		NavigationWidget *header_;
		QStackedWidget *stacked_widget_;
		StationFramePanel *frame_panel_;
		TuneWidget *tune_widget_;

		QSortFilterProxyModel *favorites_proxy_model_;
		FavoritesList *favorites_tree_;
		LineHeader *favorites_header_;

		QListView *radio_panel_;
};

#endif //NRSC5_GUI_SRC_VIEW_RADIOCONTROLPANELVIEW_H_
