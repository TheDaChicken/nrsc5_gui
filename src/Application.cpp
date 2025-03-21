//
// Created by TheDaChicken on 6/23/2024.
//

#include "Application.h"
#include "audio/PortAudioCpp.h"
#include "utils/Log.h"
#include "images/providers/LotImageProvider.h"

#include <QtConcurrentRun>
#include <QFontDatabase>
#include <QFile>
#include <QMessageBox>

Application *instance_ = nullptr;

Application::Application(int &argc, char **argv, int flags)
	: QApplication(argc, argv, flags),
	  lot_manager_(sql_manager),
	  radio_controller(this)
{
	instance_ = this;

	QFontDatabase::addApplicationFont(":/fonts/OpenSans-VariableFont.ttf");

	setApplicationName("NRSC5 GUI");
	setOrganizationName("TheDaChicken");

	sdr_system = std::make_unique<PortSDR::PortSDR>();
	port_audio = std::make_shared<PortAudio::System>();
	image_provider_ = std::make_shared<StationImageProvider>();

	tuner_devices_model_ = std::make_unique<TunerDevicesModel>(sdr_system, this);
	favorites_model_ = std::make_unique<FavoriteModel>(sql_manager, image_provider_, this);
	info_manager = std::make_unique<StationInfoManager>(image_provider_, favorites_model_);

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
	// TODO: Add error message dialog for failed initialization
	int ret = port_audio->Initialize();
	if (ret < 0)
	{
		QMessageBox msgBox;
		msgBox.setText("Failed to Open NRSC5 GUI");
		msgBox.setInformativeText("Failed to initialize PortAudio: " +
			QString::fromUtf8(PortAudio::System::ErrorText(ret)));
		msgBox.exec();
		return false;
	}

	UTILS::StatusCodes error = sql_manager.Open("database.db");
	if (error != UTILS::StatusCodes::Ok)
	{
		QMessageBox msgBox;
		msgBox.setText("Failed to Open NRSC5 GUI");
		msgBox.setInformativeText("Failed to open database: " + GetStatusMessage(error));
		msgBox.exec();
		return false;
	}

	favorites_model_->update();

	// TODO make this configurable in the settings
	ret = lot_manager_.SetImageFolder("HDImages");
	if (ret < 0)
	{
		QMessageBox msgBox;
		msgBox.setText("Failed to Open NRSC5 GUI");
		msgBox.setInformativeText("Failed to open image folder: " + QString::number(ret));
		msgBox.exec();
		return false;
	}

	image_provider_->ProviderManager()->AddProvider(
		std::make_shared<LotImageProvider>(&lot_manager_),
		1);

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
	connect(&radio_controller,
	        &RadioController::HDReceivedLot,
	        this,
	        &Application::HDReceivedLot);
	connect(&radio_controller,
	        &RadioController::TunerSyncEvent,
	        this,
	        &Application::OnAudioSyncUpdate);
	return exec();
}

/**
 * @brief This is run from the gui thread
 * @param event
 */
void Application::OnAudioSyncUpdate(const std::shared_ptr<GuiSyncEvent> &event)
{
	switch (event->GetEventType())
	{
		case GuiSyncEvent::EventType::EVENT_HD_SYNC:
		{
			const auto syncEvent = std::dynamic_pointer_cast<GuiHDSyncEvent>(event);
			emit info_manager->UpdateHDSync(syncEvent->on_);
			break;
		}
		case GuiSyncEvent::EventType::EVENT_HD_STATION:
		{
			const auto stationEvent = std::dynamic_pointer_cast<GuiStationUpdate>(event);
			info_manager->StyleAndDisplayStation(stationEvent->channel_);
			break;
		}
		case GuiSyncEvent::EventType::EVENT_HD_ID3:
		{
			const auto id3Event = std::dynamic_pointer_cast<GuiID3Update>(event);
			info_manager->StyleAndDisplayID3(id3Event->id3_);
			break;
		}
		default:
		{
			Logger::Log(warn, "Unhandled Sync Event: {}", static_cast<int>(event->GetEventType()));
			break;
		}
	}
}

/**
 * This is not run from the gui thread
 * @param station The channel that received the lot
 * @param component The component that received the lot
 * @param lot The lot that was received
 */
void Application::HDReceivedLot(const NRSC5::StationInfo &station,
                                const NRSC5::DataService &component,
                                const NRSC5::Lot &lot) const
{
	// Update GUI cache with the LOT
	info_manager->ReceiveLot(component, lot);

	// Save the lot to the database
	QFuture<void> future = QtConcurrent::run([this, component, lot, station]
	{
		lot_manager_.LotReceived(station, component, lot);
	});
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
		case UTILS::StatusCodes::UnknownError:
		default:
		{
			return tr("Unknown error");
		}
	}
}
