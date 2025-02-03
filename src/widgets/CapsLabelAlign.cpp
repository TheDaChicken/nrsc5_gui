//
// Created by TheDaChicken on 7/25/2024.
//

#include "widgets/CapsLabelAlign.h"

#include <QPainter>
#include <QStyleOption>

CapsLabelAlign::CapsLabelAlign(QWidget *parent) : QLabel(parent)
{
}

void CapsLabelAlign::paintEvent(QPaintEvent *)
{
  const QFont textFont(font());
  const QFontMetrics metrics(fontMetrics());

  QPainter painter(this);
  QStyleOption option;
  option.initFrom(this);

  QRectF contentRect(contentsRect());

  painter.setFont(textFont);
  painter.setBrush(palette().text());
  painter.setRenderHint(QPainter::TextAntialiasing);

  // Graphic here: https://stackoverflow.com/a/77836956/8075127
  // Remove the extra space from height from ascents. We want to center capital letters.
  // Took me so long to figure out that the extra space was due to the ascent.
  const qreal textCenter = contentRect.center().y();
  const int fontCenter = (metrics.height() - (metrics.capHeight() - metrics.ascent())) / 2;
  contentRect.setY((textCenter - fontCenter) / 2);

  // Draw the text centered
  painter.drawText(contentRect, 0, text());
}
