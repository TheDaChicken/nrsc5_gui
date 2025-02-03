//
// Created by TheDaChicken on 6/23/2024.
//

#ifndef NRSC5_GUI_SRC_UI_NRSC5APPLICATION_H_
#define NRSC5_GUI_SRC_UI_NRSC5APPLICATION_H_

#include <QApplication>
#include <utils/StylingText.h>

#include "windows/MainWindow.h"
#include "db/ConnectionManager.h"
#include "controllers/StationInfoManager.h"
#include "themes/ThemeManager.h"
#include "controllers/RadioController.h"
#include "models/TunerDevicesModel.h"

#define dApp Application::GetInstance()

class Application : public QApplication
{
		Q_OBJECT

	public:
		Application(int &argc, char **argv, int flags = ApplicationFlags);
		~Application() override;

		Q_DISABLE_COPY_MOVE(Application)

		void Initialize();
		int Run();

		RadioController &GetRadioController()
		{
			return radio_controller;
		}

		StationInfoManager &GetStationInfoManager()
		{
			assert(this->info_manager);
			return *this->info_manager;
		}

		ThemeManager &GetThemeManager()
		{
			return theme_manager;
		}

		StationImageProvider &GetImageProvider()
		{
			assert(this->image_provider_);
			return *this->image_provider_;
		}

		ConnectionManager &GetSQLManager()
		{
			return sql_manager;
		}

		PortAudio::System &GetAudioSystem()
		{
			assert(this->audio_system);
			return *this->audio_system;
		}

		PortSDR::PortSDR &GetSDRSystem()
		{
			assert(this->sdr_system);
			return *this->sdr_system;
		}

		LinkedChannelModel *GetFavoritesModel() const
		{
			assert(this->favorites_model_);
			return this->favorites_model_.get();
		}

		TunerDevicesModel *GetTunerDevicesModel() const
		{
			assert(this->tuner_devices_model_);
			return this->tuner_devices_model_.get();
		}

		static Application *GetInstance()
		{
			assert(instance());
			return dynamic_cast<Application *>(instance());
		}

	private slots:
		void OnAudioSyncUpdate(const std::shared_ptr<GuiSyncEvent> &event);

	private:
		void PrintStartupInformation() const;
		void HDReceivedLot(const RadioChannel &channel,
		                   const NRSC5::DataService &component, const NRSC5::Lot &lot) const;

		RadioController radio_controller;

		ConnectionManager sql_manager;
		LotManager lot_manager_;
		ThemeManager theme_manager;

		std::shared_ptr<PortAudio::System> audio_system;
		std::shared_ptr<PortSDR::PortSDR> sdr_system;

		std::shared_ptr<StationImageProvider> image_provider_;
		std::unique_ptr<StationInfoManager> info_manager;

		std::shared_ptr<LinkedChannelModel> favorites_model_;
		std::shared_ptr<TunerDevicesModel> tuner_devices_model_;
		std::unique_ptr<MainWindow> window;
};

#endif //NRSC5_GUI_SRC_UI_NRSC5APPLICATION_H_
