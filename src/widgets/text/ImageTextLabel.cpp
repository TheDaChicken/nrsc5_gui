//
// Created by TheDaChicken on 7/27/2024.
//

#include "ImageTextLabel.h"
#include <utils/Log.h>

#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>

ImageTextLabel::ImageTextLabel(QWidget *parent)
	: ImageText(parent),
	  text_width_(-1),
	  text_dry_(false),
	  layout_dry_(false)
{
}

bool ImageTextLabel::event(QEvent *event)
{
	const QEvent::Type type = event->type();

	if (type == QEvent::FontChange || type == QEvent::ApplicationFontChange)
	{
		text_document_.setDefaultFont(font());
		layout_dry_ = true;
	}
	else if (type == QEvent::Resize)
	{
		EnsureScaledPixmap();
	}
	else if (type == QEvent::Polish)
	{
		text_document_.setDefaultFont(font());
	}

	return QFrame::event(event);
}

void ImageTextLabel::EnsureTextDocument() const
{
	if (text_dry_)
	{
		text_document_.setHtml(description_);
		text_dry_ = false;
	}

	if (layout_dry_)
	{
		text_document_.setTextWidth(text_width_);
		layout_dry_ = false;
	}
}

void ImageTextLabel::mousePressEvent(QMouseEvent *event)
{
	QWidget::mousePressEvent(event);
	emit clicked();
	event->accept();
}

void ImageTextLabel::mouseMoveEvent(QMouseEvent *event)
{
	event->accept();
	QWidget::mouseMoveEvent(event);
}
