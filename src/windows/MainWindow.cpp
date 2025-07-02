//
// Created by TheDaChicken on 6/23/2024.
//

#include "MainWindow.h"

#include <qtconcurrentrun.h>

#include "ui_MainWindow.h"
#include "Application.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	  ui(new Ui::MainWindow),
	  button_group_(this)
{
	ui->setupUi(this);

	setWindowTitle("NRSC5 GUI");

	button_group_.addButton(ui->radioButton, 1);
	button_group_.addButton(ui->mapButton, 2);
	button_group_.addButton(ui->settingsButton, 3);

	connect(ui->OnButton, &QPushButton::clicked, this, &MainWindow::OnPlayButton);
	connect(&button_group_, &QButtonGroup::buttonClicked, this, &MainWindow::UpdateCurrentPage);
	connect(&getApp()->GetRadioController(),
	        &HybridRadio::TunerStatus,
	        this,
	        [this](const TunerAction action, const UTILS::StatusCodes ret)
	        {
		        if (action == TunerAction::Stop)
			        ui->OnButton->setChecked(false);
		        if (action == TunerAction::Start)
			        ui->OnButton->setChecked(true);

		        Dashboard()->UpdateTunerStatus(action, ret);
	        });

	ui->radioButton->animateClick();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::UpdateCurrentPage() const
{
	switch (button_group_.checkedId())
	{
		case 1:
		{
			SwitchToRadioDashboard();
			break;
		}
		case 3:
		{
			SwitchToSettingsPage();
			break;
		}
		default:
			break;
	}
}

void MainWindow::SwitchToRadioDashboard() const
{
	// Update the current widget to the new widget
	ui->Navigation->setCurrentWidget(ui->RadioDash);
}

void MainWindow::SwitchToSettingsPage() const
{
	ui->Navigation->setCurrentWidget(ui->Settings);
}

void MainWindow::OnPlayButton(const bool target)
{
	Q_UNUSED(this)

	QFuture<UTILS::StatusCodes> future;
	HybridRadio& radio = getApp()->GetRadioController();

	if (target)
	{
		future = QtConcurrent::run([&radio]()
		{
			return radio.StartDefault();
		});
	}
	else
	{
		future = QtConcurrent::run([&radio]()
		{
			return radio.Stop();
		});
	}

	future.then([this, target](const UTILS::StatusCodes ret)
	{
		if (ret != UTILS::StatusCodes::Ok)
		{
			// failed: back to the previous state
			ui->OnButton->setChecked(!target);
		}
	});
}

bool MainWindow::event(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::TouchBegin:
		{
			qDebug() << "touch!";
			return true;
		}
		case QEvent::TouchEnd:
		{
			qDebug() << "touch end!";
			return true;
		}
		default:
			// call base implementation
			return QMainWindow::event(event);
	}
}

RadioPage *MainWindow::Dashboard() const
{
	return ui->RadioDash;
}

SettingsPage *MainWindow::SettingsPage() const
{
	return ui->Settings;
}

QWidget *MainWindow::CurrentPage() const
{
	return ui->Navigation->currentWidget();
}
