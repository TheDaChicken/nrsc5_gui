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

Application::Application(int &argc, char **argv, int flags)
	: QApplication(argc, argv, flags),
	  radio_controller(this),
	  lot_manager_(&sql_manager)
{
	QFontDatabase::addApplicationFont(":/fonts/OpenSans-VariableFont.ttf");

	setApplicationName("NRSC5 GUI");
	setOrganizationName("TheDaChicken");

	sdr_system = std::make_unique<PortSDR::PortSDR>();
	audio_system = std::make_shared<PortAudio::System>();
	image_provider_ = std::make_shared<StationImageProvider>();

	sql_manager.SetDatabaseName("database.db");
	sql_manager.DebugLogging();

	tuner_devices_model_ = std::make_unique<TunerDevicesModel>(sdr_system, this);
	favorites_model_ = std::make_unique<LinkedChannelModel>(this,
	                                                        sql_manager.GetConnection(),
	                                                        image_provider_);
	info_manager = std::make_unique<StationInfoManager>(image_provider_, favorites_model_);

	PrintStartupInformation();
}

Application::~Application() = default;

void Application::PrintStartupInformation() const
{
	Logger::Log(debug, "PortAudio Version: {}", PortAudio::System::VersionText());
	Logger::Log(debug, "NRSC5 Version: {}", NRSC5::Decoder::VersionText());
}

void Application::Initialize()
{
	int ret;

	// TODO: Add error message dialog for failed initialization
	ret = audio_system->Initialize();
	if (ret < 0)
	{
		return;
	}

	lot_manager_.Open();
	// TODO make this configurable in the settings
	lot_manager_.SetImageFolder("HDImages");

	// TODO Remove Literal Strings and use a constant.
	//  This is from an underlying problem with how we are using QtSQL for the database and the class
	//  Move to a more robust database system like SQLite. No ORM is needed.
	favorites_model_->SetTable(QStringLiteral("FAVORITES"));

	image_provider_->ProviderManager()->AddProvider(
		std::make_shared<LotImageProvider>(&lot_manager_),
		1);

	// Load themes
	theme_manager.LoadThemes();
	// TODO: Make this configurable in the settings
	theme_manager.SetTheme(QStringLiteral("Light"));

	window = std::make_unique<MainWindow>();
	window->show();
}

int Application::Run()
{
	connect(&radio_controller,
	        &RadioController::TunerStatus,
	        this,
	        [this](const int status)
	        {
		        Logger::Log(debug, "Tuner status: {}", status);
		        window->Dashboard()->SetStatus(status);
		        window->OnSwitchPage();
	        });
	connect(&radio_controller, &RadioController::HDReceivedLot, this, &Application::HDReceivedLot);
	connect(&radio_controller, &RadioController::TunerSyncEvent, this, &Application::OnAudioSyncUpdate);

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
 * @param channel The channel that received the lot
 * @param component The component that received the lot
 * @param lot The lot that was received
 */
void Application::HDReceivedLot(const RadioChannel &channel,
                                const NRSC5::DataService &component,
                                const NRSC5::Lot &lot) const
{
	// Update GUI cache with the LOT
	info_manager->ReceiveLot(component, lot);

	// Save the lot to the database
	QFuture<void> future = QtConcurrent::run([this, component, lot, channel]()
	{
		lot_manager_.LotReceived(channel.hd_station_, component, lot);
	});
}
