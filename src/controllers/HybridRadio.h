//
// Created by TheDaChicken on 6/15/2024.
//

#ifndef NRSC5_GUI_SRC_LIB_HYBRIDRADIO_H_
#define NRSC5_GUI_SRC_LIB_HYBRIDRADIO_H_

#include "../RadioChannel.h"
#include "../nrsc5/Decoder.h"
#include "../nrsc5/Station.h"
#include "../audio/Device.h"
#include "../audio/stream/StreamOutputPush.h"
#include "../dsp/ArbResampler.h"

#include "PortSDR.h"

#include <optional>
#include <mutex>

#include "../HybridTuner.h"
#include "StationInfoManager.h"
#include "../threads/GuiSyncThread.h"
#include "../utils/Error.h"

enum class TunerAction
{
	Opening = 0, // Opening tuner
	Closing, // Closing tuner

	Open, // Open tuner
	Close, // Close tuner

	Starting, // Starting tuner stream
	Stopping, // Stopping tuner stream

	Start, // Start tuner stream
	Stop, // Stop tuner stream

	ChangeChannel, // Change channel
};

class HybridRadio final : public QObject
{
		Q_OBJECT

	public:
		explicit HybridRadio(SQLite::Database &db, QObject *parent);
		~HybridRadio() override;

		Q_DISABLE_COPY_MOVE(HybridRadio)

		UTILS::StatusCodes StartDefault();
		UTILS::StatusCodes SelectDefaultAudioDevice();
		UTILS::StatusCodes Start();
		UTILS::StatusCodes Stop();

		UTILS::StatusCodes SetSDRDevice(const PortSDR::Device &device);

		UTILS::StatusCodes ClearSDRDevice();
		UTILS::StatusCodes SetAudioDevice(const std::shared_ptr<PortAudio::Device> &device);

		UTILS::StatusCodes SetChannel(Modulation::Type type, double frequency,
		                              unsigned int programId = NRSC5_MPS_PROGRAM);
		UTILS::StatusCodes SetChannel(const Channel &channel);
		UTILS::StatusCodes SetTunerConfiguration(const TunerOpts &tunerOpts);
		void SetProgram(unsigned int programId);

		[[nodiscard]] ActiveChannel GetChannel() const;

		bool IsAudioActive() const
		{
			return audio_stream_.IsActive();
		}

		bool IsSdrActive() const
		{
			return tuner_.IsSDRActive();
		}

		StationInfoManager &GetStationInfoManager()
		{
			return station_info_manager_;
		}

		HybridTuner &GetTuner()
		{
			return tuner_;
		}

	signals:
		// Tuner signals for GUI
		void TunerStatus(TunerAction action, UTILS::StatusCodes ret);

		void TunerStationUpdate(const ActiveChannel &channel);
		void HDSignalStrength(float lower, float upper);

	private:
		void NRSC5Audio(const int16_t *data, std::size_t frame_size);
		void NRSC5Callback(const nrsc5_event_t *evt);

		void RadioStationUpdate(const ActiveChannel &channel);
		void HDSyncUpdate(bool sync);
		void HDID3Update(const NRSC5::ID3 &id3);
		void HDSignalStrengthUpdate(float lower, float upper);
		void HDReceivedLot(const NRSC5::StationInfo &station, const NRSC5::Lot &lot);
		void TunerSyncEvent(const std::shared_ptr<GuiSyncEvent> &event);
		[[nodiscard]] ActiveChannel CreateChannel() const;

		HybridTuner tuner_;

		NRSC5::StationDetails station_details_;
		NRSC5::StationInfo station_info_;
		NRSC5::Ber ber_;
		std::optional<std::chrono::time_point<std::chrono::steady_clock> > m_sync_;

		unsigned int audio_packets = 0;
		unsigned int audio_bytes = 0;

		vector_cint16_t convert_buffer_;
		vector_float_t audio_buffer_;

		PortAudio::StreamLiveOutputPush audio_stream_;

		GuiSyncThread sync_thread;
		StationInfoManager station_info_manager_;

		Channel current_channel_
		{
			{
				Modulation::Type::MOD_FM,
				92.5
			},
			0
		};

		mutable std::mutex station_mutex_;
};

#endif //NRSC5_GUI_SRC_LIB_HYBRIDRADIO_H_
