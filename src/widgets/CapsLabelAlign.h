//
// Created by TheDaChicken on 7/25/2024.
//

#ifndef NRSC5_GUI_SRC_WIDGETS_CAPSLABELALIGN_H_
#define NRSC5_GUI_SRC_WIDGETS_CAPSLABELALIGN_H_

#include <QLabel>

/**
 * @brief QLabel that capitalizes text & removes accent height.
 * Better centering of text for languages that do not use accents.
 */
class CapsLabelAlign : public QLabel
{
 Q_OBJECT
 public:
  explicit CapsLabelAlign(QWidget *parent=nullptr);
 protected:
  void paintEvent(QPaintEvent*) override;
};

#endif //NRSC5_GUI_SRC_WIDGETS_CAPSLABELALIGN_H_
