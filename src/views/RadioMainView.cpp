//
// Created by TheDaChicken on 8/14/2024.
//

#include "RadioMainView.h"
#include "Application.h"
#include "widgets/navigation/NavigationWidget.h"
#include "widgets/ListViewHeader.h"

RadioMainView::RadioMainView()
{
	cover_ = new RadioInfoDisplay();

	favorites_ = new FavoritesList(nullptr,
	                               ChannelDelegate::kDefault);
	favorites_->setObjectName("FavoritesMainView");
	favorites_->setModel(dApp->GetFavoritesModel());

	favorites_header_ = new FavoritesHeader(favorites_,
	                                        nullptr);
	favorites_header_->setObjectName("FavoritesMainViewHeader");
	favorites_header_->setFrameShadow(QFrame::Sunken);

	// Header
	header_ = new NavigationWidget();
	header_->ImageLabel()->setPixmap(QPixmap(":/buttons/BlueBack.svg"));
	header_->ViewLabel()->setText(tr("Radio"));

	SetLeftWidget(new ListViewHeader(favorites_,
	                                 favorites_header_,
	                                 nullptr));
	SetMainWidget(cover_);
	SetLeftWidgetHeader(header_);

	// "<p style=\"color: #1f71d4\">%s</p>"
}
