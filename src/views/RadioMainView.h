//
// Created by TheDaChicken on 8/14/2024.
//

#ifndef NRSC5_GUI_SRC_VIEW_RADIOMAINVIEW_H_
#define NRSC5_GUI_SRC_VIEW_RADIOMAINVIEW_H_

#include "widgets/dualview/DualViewContainer.h"
#include "widgets/navigation/NavigationWidget.h"
#include "widgets/favorites/FavoritesHeader.h"
#include "widgets/favorites/FavoritesList.h"

#include "display/RadioInfoDisplay.h"

/**
 * FavoritesList + RadioInfoDisplay
 */
class RadioMainView : public DualViewContainer
{
 public:
  explicit RadioMainView();

  [[nodiscard]] RadioInfoDisplay *Cover() const
  {
   return cover_;
  }

  [[nodiscard]] FavoritesList *FavoriteList() const
  {
   return favorites_;
  }

  [[nodiscard]] NavigationWidget *Header() const
  {
   return header_;
  }

 private:
  FavoritesList *favorites_;
  FavoritesHeader *favorites_header_;
  NavigationWidget *header_;
  RadioInfoDisplay *cover_;
};

#endif //NRSC5_GUI_SRC_VIEW_RADIOMAINVIEW_H_
