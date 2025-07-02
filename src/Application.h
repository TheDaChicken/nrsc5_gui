//
// Created by TheDaChicken on 6/23/2024.
//

#ifndef NRSC5_GUI_SRC_UI_NRSC5APPLICATION_H_
#define NRSC5_GUI_SRC_UI_NRSC5APPLICATION_H_

#include <QApplication>
#include <utils/StylingText.h>

#include "windows/MainWindow.h"
#include "controllers/StationInfoManager.h"
#include "controllers/HybridRadio.h"
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

		const HybridRadio &GetRadioController() const
		{
			return radio_controller;
		}

		HybridRadio &GetRadioController()
		{
			return radio_controller;
		}

		ThemeManager &GetThemeManager()
		{
			return theme_manager;
		}

		StationInfoManager &GetStationInfoManager()
		{
			return radio_controller.GetStationInfoManager();
		}

		StationImageProvider &GetImageProvider()
		{
			return GetStationInfoManager().GetImageProvider();
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

		PortSDR::PortSDR &GetSDRSystem() const
		{
			assert(this->sdr_system);
			return *this->sdr_system;
		}

		FavoriteModel *GetFavoritesModel()
		{
			return GetStationInfoManager().GetFavoritesModel();
		}

		TunerDevicesModel *GetTunerDevicesModel() const
		{
			assert(this->tuner_devices_model_);
			return this->tuner_devices_model_.get();
		}

	private:
		void PrintStartupInformation() const;

		SQLite::Database sql_manager;
		ThemeManager theme_manager;

		std::shared_ptr<PortAudio::System> port_audio;
		std::shared_ptr<PortSDR::PortSDR> sdr_system;

		HybridRadio radio_controller;

		std::shared_ptr<TunerDevicesModel> tuner_devices_model_;
		std::unique_ptr<MainWindow> window;
};

Application *getApp();

#endif //NRSC5_GUI_SRC_UI_NRSC5APPLICATION_H_
