//
// Created by TheDaChicken on 7/18/2024.
//

#include "FavoritesList.h"

#include <Application.h>
#include <QScrollBar>

FavoritesList::FavoritesList(QWidget *parent,
                             const ChannelDelegate::Styles style)
	: QListView(parent)
{
	setAttribute(Qt::WA_AcceptTouchEvents);

	setItemDelegate(new ChannelDelegate(this, style));
	setDragEnabled(true);
	setDragDropMode(InternalMove);
	setSelectionMode(SingleSelection);
	setSelectionBehavior(SelectRows);
	setVerticalScrollMode(ScrollPerPixel);
	verticalScrollBar()->setSingleStep(20);

	if (style == ChannelDelegate::kBoxLayout)
	{
		setFlow(LeftToRight);
		setSpacing(7);
		setWrapping(true);
		setWordWrap(true);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		setResizeMode(Adjust);
		setItemAlignment(Qt::AlignHCenter);

		setFrameShape(NoFrame);
		setFrameShadow(Plain);

		viewport()->setAutoFillBackground(false);
	}

	connect(this,
	        &FavoritesList::clicked,
	        this,
	        [this](const QModelIndex &index)
	        {
		        if (!index.isValid())
		        {
			        return;
		        }
		        getApp()->GetRadioController().SetChannel(getApp()->GetFavoritesModel()->Get(index.row()));
	        });
}
