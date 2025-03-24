//
// Created by TheDaChicken on 6/15/2024.
//

#ifndef NRSC5_GUI_SRC_LIB_HYBRIDRADIO_H_
#define NRSC5_GUI_SRC_LIB_HYBRIDRADIO_H_

#include "RadioChannel.h"
#include "nrsc5/Decoder.h"
#include "nrsc5/Station.h"
#include "audio/Device.h"
#include "audio/stream/StreamOutputPush.h"
#include "dsp/FirdecimQ15.h"

#include "PortSDR.h"

#include <optional>
#include <mutex>

#include "utils/Error.h"

class HybridRadio
{
	public:
		class Delegate
		{
			public:
				virtual ~Delegate() = default;

				virtual void SetAudioStream(const PortAudio::StreamLiveOutputPush &stream) = 0;
				virtual void RadioStationUpdate(const ActiveChannel &channel) = 0;

				virtual void HDSyncUpdate(bool sync) = 0;
				virtual void HDSignalStrengthUpdate(float lower, float upper) = 0;
				virtual void HDID3Update(const NRSC5::ID3 &id3) = 0;
				virtual void HDReceivedLot(const NRSC5::StationInfo &channel,
				                           const NRSC5::Lot &lot) = 0;
		};

		explicit HybridRadio(Delegate *delegate);

		Q_DISABLE_COPY_MOVE(HybridRadio)

		UTILS::StatusCodes Start();
		UTILS::StatusCodes Stop();

		UTILS::StatusCodes ClearSDRDevice();
		UTILS::StatusCodes SetSDRDevice(const std::shared_ptr<PortSDR::Device> &device);
		UTILS::StatusCodes SetAudioDevice(const std::shared_ptr<PortAudio::Device> &device);

		UTILS::StatusCodes SetChannel(Modulation::Type type, double frequency,
		                              unsigned int programId = NRSC5_MPS_PROGRAM);
		UTILS::StatusCodes SetChannel(const Channel &channel);
		UTILS::StatusCodes SetTunerConfiguration(const TunerOpts &tunerOpts);
		void SetProgram(unsigned int programId);

		[[nodiscard]] ActiveChannel GetChannel() const;

		bool IsSDROpened() const
		{
			return sdr_stream_ != nullptr;
		}

		bool IsAudioActive() const
		{
			return audio_stream_.IsActive();
		}

		PortSDR::Stream *GetSDRStream() const
		{
			return sdr_stream_.get();
		}

	private:
		void NRSC5Audio(const int16_t *data, std::size_t frame_size);
		void SDRCallback(const int16_t *data, std::size_t frame_size);
		[[nodiscard]] ActiveChannel CreateChannel() const;

		static void NRSC5Callback(const nrsc5_event_t *evt, void *opaque);

		std::unique_ptr<PortSDR::Stream> sdr_stream_;

		// TODO Implement better resampling for different SDRs
		std::vector<int16_t> taps = {
			static_cast<int16_t>(0.6062333583831787 * 32767), static_cast<int16_t>(-0.13481467962265015 * 32767),
			static_cast<int16_t>(0.032919470220804214 * 32767), static_cast<int16_t>(-0.00410953676328063 * 32767)
		};
		Halfband_16 halfband_16_{taps};

		Delegate *delegate_;
		NRSC5::Decoder nrsc5_decoder_;
		NRSC5::StationDetails station_details_;
		NRSC5::StationInfo station_info_;
		NRSC5::Ber ber_;
		std::optional<std::chrono::time_point<std::chrono::steady_clock> > m_sync_;

		unsigned int audio_packets = 0;
		unsigned int audio_bytes = 0;

		vector_cint16_t convert_buffer_;
		vector_float_t audio_buffer_;

		bool audio_disabled = false;
		PortAudio::StreamLiveOutputPush audio_stream_;

		mutable std::mutex station_mutex_;
};

#endif //NRSC5_GUI_SRC_LIB_HYBRIDRADIO_H_
