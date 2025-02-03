//
// Created by TheDaChicken on 6/25/2024.
//

#include "widgets/RoundButton.h"
#include "utils/Log.h"

#include <QStylePainter>
#include <QStyleOptionButton>

RoundButton::RoundButton(QWidget *parent) : QPushButton(parent)
{
	setCheckable(true);
}

void RoundButton::paintEvent(QPaintEvent *)
{
	QStylePainter painter(this);

	const int diameter = qMin(height(), width());
	const int circleBounds = diameter - (diameter / 2);

	const QPixmap scaled = icon().pixmap(circleBounds,
	                                     circleBounds,
	                                     isEnabled()
		                                     ? QIcon::Normal
		                                     : QIcon::Disabled,
	                                     isChecked()
		                                     ? QIcon::On
		                                     : QIcon::Off);

	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(Qt::NoPen);

	if (isChecked())
		painter.setBrush(QBrush(QColor(140, 181, 255, 50)));

	// Translate to the center
	painter.translate(static_cast<float>(width()) / 2.0,static_cast<float>(height()) / 2.0);

	// Draws circle
	painter.drawEllipse(QRectF(-diameter / 2.0, -diameter / 2.0, diameter, diameter));
	// Draw image
	painter.drawPixmap(QPointF(-circleBounds / 2.0, -circleBounds / 2.0), scaled);
}
