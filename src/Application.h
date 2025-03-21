//
// Created by TheDaChicken on 6/23/2024.
//

#ifndef NRSC5_GUI_SRC_UI_NRSC5APPLICATION_H_
#define NRSC5_GUI_SRC_UI_NRSC5APPLICATION_H_

#include <QApplication>
#include <utils/StylingText.h>

#include "windows/MainWindow.h"
#include "controllers/StationInfoManager.h"
#include "controllers/RadioController.h"
#include "themes/ThemeManager.h"
#include "models/TunerDevicesModel.h"
#include "sql/Database.h"

class Application : public QApplication
{
		Q_OBJECT

	public:
		Application(int &argc, char **argv, int flags = ApplicationFlags);
		~Application() override;

		Q_DISABLE_COPY_MOVE(Application)

		bool Initialize();
		int Run();
		static QString GetStatusMessage(UTILS::StatusCodes status);

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

		SQLite::Database &GetSQLManager()
		{
			return sql_manager;
		}

		PortAudio::System &GetAudioSystem()
		{
			assert(this->port_audio);
			return *this->port_audio;
		}

		PortSDR::PortSDR &GetSDRSystem()
		{
			assert(this->sdr_system);
			return *this->sdr_system;
		}

		FavoriteModel *GetFavoritesModel() const
		{
			assert(this->favorites_model_);
			return this->favorites_model_.get();
		}

		TunerDevicesModel *GetTunerDevicesModel() const
		{
			assert(this->tuner_devices_model_);
			return this->tuner_devices_model_.get();
		}

	private slots:
		void OnAudioSyncUpdate(const std::shared_ptr<GuiSyncEvent> &event);

	private:
		void PrintStartupInformation() const;
		void HDReceivedLot(const NRSC5::StationInfo &station,
		                   const NRSC5::DataService &component, const NRSC5::Lot &lot) const;

		SQLite::Database sql_manager;
		LotManager lot_manager_;
		ThemeManager theme_manager;

		std::shared_ptr<PortAudio::System> port_audio;
		std::shared_ptr<PortSDR::PortSDR> sdr_system;

		std::shared_ptr<StationImageProvider> image_provider_;
		std::unique_ptr<StationInfoManager> info_manager;

		RadioController radio_controller;

		std::shared_ptr<FavoriteModel> favorites_model_;
		std::shared_ptr<TunerDevicesModel> tuner_devices_model_;
		std::unique_ptr<MainWindow> window;
};

Application *getApp();

#endif //NRSC5_GUI_SRC_UI_NRSC5APPLICATION_H_
