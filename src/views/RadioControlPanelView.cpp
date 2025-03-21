//
// Created by TheDaChicken on 8/15/2024.
//

#include "RadioControlPanelView.h"
#include "Application.h"

#include <QListView>
#include <QStringListModel>
#include <QSortFilterProxyModel>

#include "widgets/StationFramePanel.h"
#include "widgets/ListViewHeader.h"
#include "delegates/BasicTextDelegate.h"
#include "delegates/ChannelDelegate.h"

RadioControlPanelView::RadioControlPanelView()
{
	QStringList list;
	list << tr("Favorites") << tr("Tune") << tr("FM Radio") << tr("AM Radio");

	stacked_widget_ = new QStackedWidget();
	stacked_widget_->setFrameShadow(QFrame::Plain);
	stacked_widget_->setFrameShape(QFrame::NoFrame);

	favorites_proxy_model_ = new QSortFilterProxyModel(this);
	favorites_proxy_model_->setSourceModel(getApp()->GetFavoritesModel());

	favorites_tree_ = new FavoritesList(nullptr, ChannelDelegate::kBoxLayout);
	favorites_tree_->setObjectName("FavoritesCubed");
	favorites_tree_->setContentsMargins(20, 0, 20, 0);
	favorites_tree_->setModel(favorites_proxy_model_);

	favorites_header_ = new LineHeader();
	favorites_header_->setObjectName("FavoritesCubedHeader");
	favorites_header_->SetText(tr("Favorites"), "");

	frame_panel_ = new StationFramePanel(stacked_widget_);
	frame_panel_->AddWidget(new ListViewHeader(favorites_tree_,
	                                           favorites_header_,
	                                           nullptr));

	tune_widget_ = new TuneWidget(stacked_widget_);

	stacked_widget_->addWidget(frame_panel_);
	stacked_widget_->addWidget(tune_widget_);

	radio_panel_ = new QListView();
	radio_panel_->setItemDelegate(new BasicTextDelegate(radio_panel_));
	radio_panel_->setModel(new QStringListModel(list));
	radio_panel_->setResizeMode(QListWidget::Adjust);
	radio_panel_->setCurrentIndex(radio_panel_->indexAt(QPoint(1, 0)));
	radio_panel_->setSelectionMode(QAbstractItemView::SingleSelection);
	radio_panel_->setSelectionBehavior(QAbstractItemView::SelectRows);
	radio_panel_->setEditTriggers(QAbstractItemView::NoEditTriggers);
	radio_panel_->setFocusPolicy(Qt::NoFocus);

	// Header
	header_ = new NavigationWidget();
	header_->ImageLabel()->setPixmap(getApp()->GetImageProvider().DefaultRadio());
	header_->ViewLabel()->setText(tr("Radio"));

	SetLeftWidget(radio_panel_);
	SetLeftWidgetHeader(header_);
	SetMainWidget(stacked_widget_);

	connect(radio_panel_, &QListView::clicked, this, &RadioControlPanelView::OnRadioPanelClicked);
}

void RadioControlPanelView::OnRadioPanelClicked(const QModelIndex &index) const
{
	switch (index.row())
	{
		case 0:
			stacked_widget_->setCurrentIndex(0);
			break;
		case 1:
			stacked_widget_->setCurrentIndex(1);
			break;
		default:
			break;
	}
}
