//
// Created by TheDaChicken on 6/23/2024.
//

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Application.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow), button_group_(this)
{
	ui->setupUi(this);

	setWindowTitle("NRSC5 GUI");

	button_group_.addButton(ui->radioButton, 1);
	button_group_.addButton(ui->mapButton, 2);
	button_group_.addButton(ui->settingsButton, 3);

	connect(ui->OnButton, &QPushButton::clicked, this, &MainWindow::OnPlayButton);
	connect(&button_group_, &QButtonGroup::buttonClicked, this, &MainWindow::OnSwitchPage);

	ui->radioButton->animateClick();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::OnSwitchPage() const
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
	// Update the RadioDashboard
	ui->RadioDash->Update();

	// Update the current widget to the new widget
	ui->Navigation->setCurrentWidget(ui->RadioDash);
}

void MainWindow::SwitchToSettingsPage() const
{
	ui->Navigation->setCurrentWidget(ui->Settings);
}

void MainWindow::OnPlayButton(const bool target) const
{
	Q_UNUSED(this)

	int ret;

	if (target)
	{
		ret = dApp->GetRadioController().StartTuner();
	}
	else
	{
		ret = dApp->GetRadioController().StopTuner();
	}

	if (ret < 0)
	{
		// failed: back to the previous state
		ui->OnButton->setChecked(!target);
	}
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

RadioDashboard *MainWindow::Dashboard() const
{
	return ui->RadioDash;
}

QWidget *MainWindow::CurrentPage() const
{
	return ui->Navigation->currentWidget();
}
