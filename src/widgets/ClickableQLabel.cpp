//
// Created by TheDaChicken on 9/12/2024.
//

#include "ClickableQLabel.h"

ClickableQLabel::ClickableQLabel(QWidget *parent) : QLabel(parent)
{

}

void ClickableQLabel::mousePressEvent(QMouseEvent *event)
{
	QLabel::mousePressEvent(event);
	emit clickable();
}
