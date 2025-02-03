//
// Created by TheDaChicken on 6/25/2024.
//

#ifndef NRSC5_GUI_SRC_UI_WIDGETS_ROUNDBUTTON_H_
#define NRSC5_GUI_SRC_UI_WIDGETS_ROUNDBUTTON_H_

#include <QPushButton>

class RoundButton : public QPushButton
{
 Q_OBJECT
 public:
  explicit RoundButton(QWidget *parent = nullptr);
  Q_DISABLE_COPY(RoundButton);

 protected:
  void paintEvent(QPaintEvent *) override;
};

#endif //NRSC5_GUI_SRC_UI_WIDGETS_ROUNDBUTTON_H_
