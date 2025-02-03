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
				void RadioStationUpdate(const RadioChannel &channel) override;

				void HDSyncUpdate(bool sync) override;
				void HDSignalStrengthUpdate(float lower, float upper) override;
				void HDID3Update(const NRSC5::ID3 &id3) override;
				void HDReceivedLot(const RadioChannel &channel,
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
		int StartTuner();
		int StopTuner();

		QFuture<int> SetSDRDevice(const std::shared_ptr<PortSDR::Device> &device);

		int SetChannel(const RadioChannel &channel)
		{
			return radio_.SetRadioChannel(channel);
		}

		void SetProgram(unsigned int programId)
		{
			radio_.SetProgram(programId);
		}

		HybridRadio &GetRadio()
		{
			return radio_;
		}

		/**
		 * @brief Get the current radio channel
		 * @return The current radio channel
		 */
		RadioChannel GetChannel() const
		{
			return radio_.GetChannel();
		}

	signals:
		// Tuner signals for GUI
		void TunerStatus(int status);

		void TunerStationUpdate(const RadioChannel &channel);
		void TunerSyncEvent(const std::shared_ptr<GuiSyncEvent> &event);

		void HDReceivedLot(const RadioChannel &channel, const NRSC5::DataService &component,
		                   const NRSC5::Lot &lot);
		void HDSignalStrength(float lower, float upper);

	private:
		int SetupDevices();
		int SetupTuner(Modulation::Type type, double freq);

		GuiDelegate delegate_;
		HybridRadio radio_;

		GuiSyncThread sync_thread;
		QFuture<int> tuner_switch_future;
		QFutureWatcher<int> tuner_watcher;
};

#endif //NRSC5_GUI_SRC_RADIOCONTROLLER_H_
