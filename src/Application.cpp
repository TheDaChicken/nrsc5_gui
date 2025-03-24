//
// Created by TheDaChicken on 6/23/2024.
//

#include "Application.h"
#include "audio/PortAudioCpp.h"
#include "utils/Log.h"
#include "images/providers/LotImageProvider.h"

#include <QFontDatabase>
#include <QFile>
#include <QMessageBox>

Application *instance_ = nullptr;

Application::Application(int &argc, char **argv, int flags)
	: QApplication(argc, argv, flags),
	  radio_controller(sql_manager, this)
{
	instance_ = this;

	QFontDatabase::addApplicationFont(":/fonts/OpenSans-VariableFont.ttf");

	setApplicationName("NRSC5 GUI");
	setOrganizationName("TheDaChicken");

	sdr_system = std::make_unique<PortSDR::PortSDR>();
	port_audio = std::make_shared<PortAudio::System>();
	tuner_devices_model_ = std::make_unique<TunerDevicesModel>(sdr_system, this);

	PrintStartupInformation();
}

Application::~Application()
{
	instance_ = nullptr;
}

void Application::PrintStartupInformation() const
{
	Logger::Log(debug, "PortAudio Version: {}", PortAudio::System::VersionText());
	Logger::Log(debug, "NRSC5 Version: {}", NRSC5::Decoder::VersionText());
}

bool Application::Initialize()
{
	if (PaError ret = port_audio->Initialize();
		ret < 0)
	{
		QMessageBox msgBox;
		msgBox.setText("Failed to Open NRSC5 GUI");
		msgBox.setInformativeText("Failed to initialize PortAudio: " +
			QString::fromUtf8(PortAudio::System::ErrorText(ret)));
		msgBox.exec();
		return false;
	}

	if (const UTILS::StatusCodes error = sql_manager.Open("database.db");
		error != UTILS::StatusCodes::Ok)
	{
		QMessageBox msgBox;
		msgBox.setText("Failed to Open NRSC5 GUI");
		msgBox.setInformativeText("Failed to open database: " + GetStatusMessage(error));
		msgBox.exec();
		return false;
	}

	GetFavoritesModel()->update();

	// TODO make this configurable in the settings
	if (std::error_code fileSystem = GetStationInfoManager().GetLotManager().SetImageFolder("HDImages"))
	{
		QMessageBox msgBox;
		msgBox.setText("Failed to Open NRSC5 GUI");
		msgBox.setInformativeText("Failed to open image folder: " + QString::fromStdString(fileSystem.message()));
		msgBox.exec();
		return false;
	}

	// Load themes
	theme_manager.LoadThemes();
	// TODO: Make this configurable in the settings
	theme_manager.SetTheme(QStringLiteral("Light"));

	window = std::make_unique<MainWindow>();
	window->show();
	return true;
}

int Application::Run()
{
	return exec();
}

Application *getApp()
{
	assert(instance_ != nullptr);

	return instance_;
}

QString Application::GetStatusMessage(const UTILS::StatusCodes status)
{
	switch (status)
	{
		case UTILS::StatusCodes::NoDevice:
			return tr("No SDR device found");
		case UTILS::StatusCodes::TunerError:
			return tr("Bad communication with SDR device");
		case UTILS::StatusCodes::SQLBusy:
			return tr("SQL database is busy");
		case UTILS::StatusCodes::DatabaseError:
			return tr("Database error");
		case UTILS::StatusCodes::NoPermission:
			return tr("No permission to folder");
		case UTILS::StatusCodes::UnknownError:
		default:
		{
			return tr("Unknown error");
		}
	}
}
