//
// Created by TheDaChicken on 10/13/2024.
//

#include "AboutPage.h"

#include <QTextEdit>
#include <QVBoxLayout>

AboutPage::AboutPage(QWidget *parent) : QWidget(parent)
{
	layout = new QVBoxLayout(this);
	layout->setContentsMargins(5, 5, 5, 5);

	text = new QTextEdit(this);
	text->setObjectName("AboutText");
	text->setReadOnly(true);
	text->setHtml(
		"<h1>NRSC5 GUI</h1>"
		"<p>nrsc5 GUI is a graphical user interface for libnrsc5. It is designed to be user-friendly and easy to use.</p>"
		"<p>libnrsc5 is a C library for decoding HD Radio signals from a RTL-SDR.</p>"
		"<p>Created by TheDaChicken</p>"
		"<p>Version 1.0</p>"
	);
	text->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	text->setFrameShadow(QFrame::Plain);
	text->setFrameShape(QFrame::NoFrame);

	QPalette palette = text->palette();
	palette.setColor(QPalette::Base, Qt::transparent);
	text->setPalette(palette);

	layout->addWidget(text);
}

AboutPage::~AboutPage() = default;
