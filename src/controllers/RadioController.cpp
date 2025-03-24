//
// Created by TheDaChicken on 9/2/2024.
//

#include <QtConcurrentRun>

#include "RadioController.h"

#include <Application.h>

#include "utils/Log.h"
#include "utils/Error.h"
#include "audio/PortAudioCpp.h"

RadioController::RadioController(
	SQLite::Database &db, QObject *parent)
	: QObject(parent),
	  delegate_(this, &sync_thread),
	  radio_(&delegate_),
	  station_info_manager_(db)
{
	connect(&sync_thread,
	        &GuiSyncThread::SyncEvent,
	        this,
	        &RadioController::TunerSyncEvent);
	sync_thread.start();
}

UTILS::StatusCodes RadioController::StartTuner()
{
	const QFuture future = QtConcurrent::run(
				[this](const Channel &radio_channel)
				{
					UTILS::StatusCodes ret = SetupDevices();
					if (ret != UTILS::StatusCodes::Ok)
						return ret;

					ret = radio_.SetChannel(radio_channel);
					if (ret != UTILS::StatusCodes::Ok)
						return ret;

					ret = radio_.Start();
					if (ret != UTILS::StatusCodes::Ok)
						return ret;

					return UTILS::StatusCodes::Ok;
				},
				current_channel_)
			.then(QtFuture::Launch::Inherit,
			      [this](const UTILS::StatusCodes ret)
			      {
				      emit TunerStatus(TunerAction::Started, ret);
			      });

	emit TunerStatus(TunerAction::Starting, UTILS::StatusCodes::Ok);
	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes RadioController::StopTuner()
{
	QFuture future = QtConcurrent::run([this] { return radio_.Stop(); })
			.then(QtFuture::Launch::Inherit,
			      [this](const UTILS::StatusCodes ret)
			      {
				      emit TunerStatus(TunerAction::Stopped, ret);
			      });

	emit TunerStatus(TunerAction::Stopping, UTILS::StatusCodes::Ok);
	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes RadioController::SetSDRDevice(const std::shared_ptr<PortSDR::Device> &device)
{
	ClearSDRDevice();

	QFuture future = QtConcurrent::run([this, device]
	{
		return radio_.SetSDRDevice(device);
	}).then(QtFuture::Launch::Inherit,
	        [this](const UTILS::StatusCodes ret)
	        {
		        emit TunerStatus(TunerAction::Stopped, ret);
		        emit TunerStream(radio_.GetSDRStream());
	        });

	emit TunerStatus(TunerAction::Starting, UTILS::StatusCodes::Ok);
	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes RadioController::ClearSDRDevice()
{
	radio_.ClearSDRDevice();
	emit TunerStatus(TunerAction::Stopped, UTILS::StatusCodes::Ok);
	emit TunerStream(nullptr);
	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes RadioController::SetupDevices()
{
	if (!radio_.IsSDROpened())
		return UTILS::StatusCodes::NoDevice;

	// TODO: Implement a way to select the default audio device.
	//  That way, this function isn't needed. This is a temporary solution.
	std::shared_ptr<PortAudio::Device> audioDefaultDevice;

	/* select audio device if not chosen */
	getApp()->GetAudioSystem().DefaultOutputDevice(audioDefaultDevice);

	UTILS::StatusCodes ret = radio_.SetAudioDevice(audioDefaultDevice);
	if (ret != UTILS::StatusCodes::Ok)
		return ret;

	Logger::Log(info,
	            "Selected Audio Device Automatically: {}",
	            audioDefaultDevice->Name());
	return UTILS::StatusCodes::Ok;
}

void RadioController::Close()
{
	if (radio_.IsAudioActive())
		radio_.Stop();
	sync_thread.Stop();
}

/**
 * Provides the audio stream of the radio
 * @param stream
 */
void RadioController::GuiDelegate::SetAudioStream(const PortAudio::StreamLiveOutputPush &stream)
{
	sync_thread->SetStream(&stream);
	stream_ = &stream;
}

PaTime RadioController::GuiDelegate::GetWrittenTime() const
{
	if (!this->stream_)
		return 0;

	return this->stream_->GetWrittenTime();
}

/**
 * @brief Automatically ran when the radio channel is updated
 * This is not ran from the gui thread
 * @param channel
 */
void RadioController::GuiDelegate::RadioStationUpdate(const ActiveChannel &channel)
{
	sync_thread->Put(std::make_unique<GuiStationUpdate>(GetWrittenTime(), channel));
}

void RadioController::GuiDelegate::HDSyncUpdate(bool sync)
{
	sync_thread->Put(std::make_unique<GuiHDSyncEvent>(GetWrittenTime(), sync));
}

void RadioController::GuiDelegate::HDID3Update(const NRSC5::ID3 &id3)
{
	sync_thread->Put(std::make_unique<GuiID3Update>(GetWrittenTime(), id3));
}

void RadioController::GuiDelegate::HDSignalStrengthUpdate(float lower, float upper)
{
	emit controller_->HDSignalStrength(lower, upper);
}

void RadioController::GuiDelegate::HDReceivedLot(const NRSC5::StationInfo &station,
                                                 const NRSC5::Lot &lot)
{
	// Update GUI cache with the LOT
	controller_->station_info_manager_.ReceiveLot(station, lot);
}

void RadioController::TunerSyncEvent(const std::shared_ptr<GuiSyncEvent> &event)
{
	switch (event->GetEventType())
	{
		case GuiSyncEvent::EventType::EVENT_HD_SYNC:
		{
			const auto syncEvent = std::dynamic_pointer_cast<GuiHDSyncEvent>(event);
			emit station_info_manager_.UpdateHDSync(syncEvent->on_);
			break;
		}
		case GuiSyncEvent::EventType::EVENT_HD_STATION:
		{
			const auto stationEvent = std::dynamic_pointer_cast<GuiStationUpdate>(event);
			station_info_manager_.StyleAndDisplayStation(stationEvent->channel_);
			break;
		}
		case GuiSyncEvent::EventType::EVENT_HD_ID3:
		{
			const auto id3Event = std::dynamic_pointer_cast<GuiID3Update>(event);
			station_info_manager_.StyleAndDisplayID3(id3Event->id3_);
			break;
		}
		default:
		{
			Logger::Log(warn, "Unhandled Sync Event: {}", static_cast<int>(event->GetEventType()));
			break;
		}
	}
}
