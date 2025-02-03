//
// Created by TheDaChicken on 7/18/2024.
//

#ifndef NRSC5_GUI_SRC_WIDGETS_FAVORITESLIST_H_
#define NRSC5_GUI_SRC_WIDGETS_FAVORITESLIST_H_

#include <QListView>

#include "delegates/ChannelDelegate.h"

/**
 * @brief Vertical Channel list widget that handles list of favorite channels.
 */
class FavoritesList : public QListView
{
 public:
  explicit FavoritesList(QWidget *parent = nullptr,
                         ChannelDelegate::Styles style = ChannelDelegate::kDefault);
};

#endif //NRSC5_GUI_SRC_WIDGETS_FAVORITESLIST_H_
