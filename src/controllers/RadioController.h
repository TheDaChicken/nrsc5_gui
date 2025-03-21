//
// Created by TheDaChicken on 9/2/2024.
//

#ifndef NRSC5_GUI_SRC_RADIOCONTROLLER_H_
#define NRSC5_GUI_SRC_RADIOCONTROLLER_H_

#include <QObject>
#include <QFutureWatcher>
#include <QFuture>

#include "RadioChannel.h"
#include "HybridRadio.h"
#include "threads/GuiEvent.h"
#include "threads/GuiSyncThread.h"
#include "utils/Error.h"

enum class TunerAction
{
	Stopped = 0,
	Starting = 1,
	Stopping = 2,
	Started = 3
};

/**
 * @brief Controller for the radio tuner and the audio stream
 * Functions here are meant to prevent the GUI from freezing by using separate threads
 */
class RadioController : public QObject
{
		Q_OBJECT

	public:
		class GuiDelegate : public HybridRadio::Delegate
		{
			public:
				GuiDelegate(RadioController *controller, GuiSyncThread *sync_thread)
					: controller_(controller), sync_thread(sync_thread)
				{
					assert(this->sync_thread);
				}

				[[nodiscard]] PaTime GetWrittenTime() const;

				void SetAudioStream(const PortAudio::StreamLiveOutputPush &stream) override;
				void RadioStationUpdate(const ActiveChannel &channel) override;

				void HDSyncUpdate(bool sync) override;
				void HDSignalStrengthUpdate(float lower, float upper) override;
				void HDID3Update(const NRSC5::ID3 &id3) override;
				void HDReceivedLot(const NRSC5::StationInfo &channel,
				                   const NRSC5::DataService &component,
				                   const NRSC5::Lot &lot) override;

			private:
				const PortAudio::StreamLiveOutputPush *stream_{nullptr};
				RadioController *controller_;
				GuiSyncThread *sync_thread;
		};

		explicit RadioController(QObject *parent);

		~RadioController() override
		{
			Close();
		}

		void Close();

		/**
		 * @brief Tries to start the tuner on a separate thread
		 */
		UTILS::StatusCodes StartTuner();
		UTILS::StatusCodes StopTuner();

		UTILS::StatusCodes SetSDRDevice(const std::shared_ptr<PortSDR::Device> &device);
		UTILS::StatusCodes ClearSDRDevice();

		UTILS::StatusCodes SetChannel(const Channel &channel)
		{
			return radio_.SetChannel(channel);
		}
		UTILS::StatusCodes SetChannel(const Modulation::Type type, const double freq)
		{
			return radio_.SetChannel(type, freq);
		}
		void SetProgram(const unsigned int programId)
		{
			radio_.SetProgram(programId);
		}

		/**
		 * @brief Get the current radio channel
		 * @return The current radio channel
		 */
		ActiveChannel GetActiveChannel() const
		{
			return radio_.GetChannel();
		}

		HybridRadio &GetRadio()
		{
			// TODO: Get rid of this function
			return radio_;
		}

	signals:
		// Tuner signals for GUI
		void TunerStatus(TunerAction action, UTILS::StatusCodes ret);
		void TunerStream(PortSDR::Stream *stream);

		void TunerStationUpdate(const ActiveChannel &channel);
		void TunerSyncEvent(const std::shared_ptr<GuiSyncEvent> &event);

		void HDReceivedLot(const NRSC5::StationInfo &station, const NRSC5::DataService &component,
		                   const NRSC5::Lot &lot);
		void HDSignalStrength(float lower, float upper);

	private:
		UTILS::StatusCodes SetupDevices();

		GuiDelegate delegate_;
		GuiSyncThread sync_thread;
		HybridRadio radio_;

		Channel current_channel_
		{
			{
				Modulation::Type::MOD_FM,
				88.5
			},
			0
		};
};

#endif //NRSC5_GUI_SRC_RADIOCONTROLLER_H_
