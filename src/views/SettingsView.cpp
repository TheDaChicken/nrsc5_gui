//
// Created by TheDaChicken on 10/8/2024.
//

#include "SettingsView.h"

#include <QListWidget>
#include <QStackedWidget>
#include <QStringListModel>
#include <delegates/BasicTextDelegate.h>

#include "RadioMainView.h"

SettingsView::SettingsView()
{
	QStringList list;

	list << tr("Appearance")
			<< tr("Tuner")
			<< tr("Network")
			<< tr("HDRadio")
			<< tr("Audio")
			<< tr("About");

	stacked_widget_ = new QStackedWidget();
	stacked_widget_->setFrameShadow(QFrame::Plain);
	stacked_widget_->setFrameShape(QFrame::NoFrame);

	about_page_ = new AboutPage(stacked_widget_);
	about_page_->setObjectName("AboutPage");
	about_page_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	tuner_page_ = new TunerPage(stacked_widget_);
	tuner_page_->setObjectName("TunerPage");
	tuner_page_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	stacked_widget_->addWidget(about_page_);
	stacked_widget_->addWidget(tuner_page_);

	settings_panel_ = new QListView();
	settings_panel_->setItemDelegate(new BasicTextDelegate(settings_panel_));
	settings_panel_->setModel(new QStringListModel(list));
	settings_panel_->setResizeMode(QListWidget::Adjust);
	settings_panel_->setSelectionMode(QAbstractItemView::SingleSelection);
	settings_panel_->setSelectionBehavior(QAbstractItemView::SelectRows);
	settings_panel_->setEditTriggers(QAbstractItemView::NoEditTriggers);
	settings_panel_->setFocusPolicy(Qt::NoFocus);

	SetLeftWidget(settings_panel_);
	SetMainWidget(stacked_widget_);

	connect(settings_panel_, &QListView::clicked, this, &SettingsView::SwitchPage);
}

TunerPage * SettingsView::GetTunerPage() const
{
	assert(tuner_page_);

	return tuner_page_;
}

void SettingsView::SwitchPage(const QModelIndex &index) const
{
	switch (index.row())
	{
		case 0:
			break;
		case 1:
		{
			stacked_widget_->setCurrentWidget(tuner_page_);
			break;
		}
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
		{
			stacked_widget_->setCurrentWidget(about_page_);
			break;
		}
		default:
			break;
	}
}
