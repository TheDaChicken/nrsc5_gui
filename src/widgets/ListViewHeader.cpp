//
// Created by TheDaChicken on 9/5/2024.
//

#include <QVBoxLayout>

#include "ListViewHeader.h"

ListViewHeader::ListViewHeader(QListView *list_view, LineHeader *header, QWidget *parent)
	: QFrame(parent),
	  header_(header),
	  list_(list_view)
{
	layout_ = new QVBoxLayout(this);

	layout_->addWidget(header_);
	layout_->addWidget(list_);

	layout_->setSpacing(0);
	layout_->setContentsMargins(0, 0, 0, 0);

	setFrameShadow(Plain);
	setFrameShape(NoFrame);
}
