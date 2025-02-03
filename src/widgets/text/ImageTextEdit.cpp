//
// Created by TheDaChicken on 9/16/2024.
//

#include "ImageTextEdit.h"

#include <qcoreevent.h>

ImageTextEdit::ImageTextEdit(QWidget *parent)
	: ImageText(parent)
{
	connect(&text_document_,
	        &QTextDocument::contentsChanged,
	        this,
	        [this]
	        {
		        update();
	        });
}

bool ImageTextEdit::event(QEvent *event)
{
	const QEvent::Type type = event->type();

	if (type == QEvent::Polish
		|| type == QEvent::FontChange || type == QEvent::ApplicationFontChange)
	{
		text_document_.setDefaultFont(font());
	}

	return QFrame::event(event);
}
