//
// Created by TheDaChicken on 9/5/2024.
//

#ifndef NRSC5_GUI_LIB_CHANNELS_FAVORITESFRAME_H_
#define NRSC5_GUI_LIB_CHANNELS_FAVORITESFRAME_H_

#include <QFrame>

#include "widgets/favorites/FavoritesList.h"
#include "widgets/favorites/FavoritesHeader.h"

/**
 * @brief A QFrame that contains a List and a Header.
 */
class ListViewHeader : public QFrame
{
 public:
  explicit ListViewHeader(QListView *list_view, LineHeader *header, QWidget *parent = nullptr);

  [[nodiscard]] QListView *GetList() const
  {
   return list_;
  }
  [[nodiscard]] LineHeader *GetHeader() const
  {
   return header_;
  }
  [[nodiscard]] QVBoxLayout *GetLayout() const
  {
   return layout_;
  }

 private:
  QVBoxLayout *layout_;

  LineHeader *header_;
  QListView *list_;
};

#endif //NRSC5_GUI_LIB_CHANNELS_FAVORITESFRAME_H_
