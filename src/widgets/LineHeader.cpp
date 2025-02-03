//
// Created by TheDaChicken on 9/6/2024.
//

#include "widgets/LineHeader.h"

#include <QPainter>

#include "delegates/ChannelDelegate.h"

LineHeader::LineHeader(QWidget *parent)
	: QFrame(parent)
{
}

void LineHeader::SetText(const QString &leftText, const QString &rightText)
{
	leftText_ = leftText;
	rightText_ = rightText;
	update();
}

void LineHeader::paintEvent(QPaintEvent *event)
{
	// Constants for the header text
	// Margin around the text in the header (left and right)
	const QRect contentRect(contentsRect().adjusted(
		kHeaderMargin,
		0,
		-kHeaderMargin,
		0
	));
	const QPalette kPalette = palette();

	QPainter painter(this);
	QFont textFont = font();

	painter.save();
	painter.setRenderHint(QPainter::Antialiasing);

	painter.setBrush(kPalette.text());
	painter.setFont(textFont);

	// Draw the header text
	painter.drawText(contentRect.adjusted(
		                 kHeaderHozMarginText,
		                 0,
		                 0,
		                 -kHeaderThickness),
	                 Qt::AlignLeft | Qt::AlignVCenter,
	                 leftText_);

	// FIXME: This side has less margin. why?? the text is right-aligned ??
	painter.drawText(contentRect.adjusted(0,
	                                      0,
	                                      -(kHeaderHozMarginText + 5),
	                                      -kHeaderThickness),
	                 Qt::AlignRight | Qt::AlignVCenter,
	                 rightText_);

	// Draw separator line at the bottom
	painter.setPen({kPalette.mid(), kHeaderThickness});
	painter.drawLine(contentRect.bottomLeft(), contentRect.bottomRight());

	painter.restore();
}

QSize LineHeader::sizeHint() const
{
	const QFontMetrics font_metrics = fontMetrics();
	const QSize rect = font_metrics.size(0, leftText_);

	return {
		rect.width(),
		rect.height() + (kHeaderThickness * 4)
	};
}
