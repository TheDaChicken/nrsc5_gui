//
// Created by TheDaChicken on 8/8/2024.
//

#ifndef NRSC5_GUI_SRC_PAGES_RADIOCOVER_H_
#define NRSC5_GUI_SRC_PAGES_RADIOCOVER_H_

#include <QFrame>

#include "RadioChannel.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
class RadioCoverLayout;
}

QT_END_NAMESPACE

class RadioInfoDisplay : public QFrame
{
  Q_OBJECT

 public:
  explicit RadioInfoDisplay(QWidget *parent = nullptr);
  ~RadioInfoDisplay() override;

  void DisplayStationLogo(const QPixmap &logo) const;
  void DisplayPrimaryImage(const QPixmap &image) const;
  void DisplayFavorite(const QModelIndex &index) const;
  void DisplaySignalMeter(float lower, float upper);
  void ClearID3() const;

  void DisplayHDSync(bool sync) const;
  void DisplayChannel(const RadioChannel &channel) const;
  void DisplayHDID3(const NRSC5::ID3 &id3) const;

 private slots:
  void OnPlusButton();
  void OnMinusButton();
  void OnFavoriteButton(bool status);

 private:
  void OnFavoriteRemoved(const QModelIndex &parent, int first, int last);
  void OnFavoriteAdded(const QModelIndex &parent, int first, int last);

  Ui::RadioCoverLayout *ui;
};

#endif //NRSC5_GUI_SRC_PAGES_RADIOCOVER_H_
