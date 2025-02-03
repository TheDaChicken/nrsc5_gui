//
// Created by TheDaChicken on 9/2/2024.
//

#include <QtConcurrentRun>

#include "RadioController.h"

#include <Application.h>
#include <QtConcurrent>

#include "utils/Log.h"
#include "utils/Error.h"
#include "audio/PortAudioCpp.h"

PaTime RadioController::GuiDelegate::GetWrittenTime() const
{
	if (this->stream_)
	{
		return this->stream_->GetWrittenTime();
	}

	return 0;
}

RadioController::RadioController(QObject *parent)
	: QObject(parent),
	  delegate_(this, &sync_thread),
	  radio_(&delegate_)
{
	connect(&sync_thread,
	        &GuiSyncThread::SyncEvent,
	        this,
	        &RadioController::TunerSyncEvent);
	connect(&tuner_watcher,
			&QFutureWatcher<int>::finished,
			this,
			[this]
			{
				emit TunerStatus(tuner_switch_future.result());
			});

	sync_thread.start();
}

int RadioController::StartTuner()
{
	if (tuner_switch_future.isRunning())
	{
		Logger::Log(warn, "Tuner thread is already running");
		return NRSC5_DEFAULT_ERROR;
	}

	tuner_switch_future = QtConcurrent::run(
		[this](const Modulation::Type type, const double freq)
		{
			return SetupTuner(type, freq);
		},
		Modulation::Type::MOD_FM,
		88.5);
	tuner_watcher.setFuture(tuner_switch_future);

	emit TunerStatus(NRSC5_TUNER_STARTING);
	return 0;
}

int RadioController::StopTuner()
{
	if (tuner_switch_future.isRunning())
	{
		Logger::Log(warn, "Tuner thread is already running");
		return NRSC5_DEFAULT_ERROR;
	}

	tuner_switch_future = QtConcurrent::run([this]
	{
		radio_.Stop();
		return NRSC5_TUNER_NOT_ACTIVE;
	});
	tuner_watcher.setFuture(tuner_switch_future);

	emit TunerStatus(NRSC5_TUNER_STOPPING);
	return 0;
}

QFuture<int> RadioController::SetSDRDevice(const std::shared_ptr<PortSDR::Device> &device)
{
	QFuture<int> device_set_future = QtConcurrent::run([this, device]
	{
		return radio_.SetSDRDevice(device);
	});

	QFutureWatcher<int> device_pending_watcher;
	device_pending_watcher.setFuture(device_set_future);

	connect(&device_pending_watcher,
	        &QFutureWatcher<int>::finished,
	        this,
	        [this]
	        {
		        emit TunerStatus(NRSC5_TUNER_NOT_ACTIVE);
	        });

	emit TunerStatus(NRSC5_TUNER_DEVICE_PENDING);
	return device_set_future;
}

int RadioController::SetupDevices()
{
	std::shared_ptr<PortAudio::Device> audioDefaultDevice;

	if (!radio_.IsOpened())
	{
		return NRSC5_SDR_NO_DEVICE;
	}

	/* select audio device if not chosen */
	dApp->GetAudioSystem().DefaultOutputDevice(audioDefaultDevice);

	int ret = radio_.SetAudioDevice(audioDefaultDevice);
	if (ret < 0)
		return ret;

	Logger::Log(info,
	            "Selected Audio Device Automatically: {}",
	            audioDefaultDevice->Name());
	return 0;
}

int RadioController::SetupTuner(const Modulation::Type type, const double freq)
{
	// Setup devices
	int ret = SetupDevices();
	if (ret < 0)
	{
		return ret;
	}

	ret = radio_.SetChannel(type, freq);
	if (ret < 0)
	{
		return ret;
	}

	return radio_.Start();
}

void RadioController::Close()
{
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

/**
 * @brief Automatically ran when the radio channel is updated
 * This is not ran from the gui thread
 * @param channel
 */
void RadioController::GuiDelegate::RadioStationUpdate(const RadioChannel &channel)
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

void RadioController::GuiDelegate::HDReceivedLot(const RadioChannel &channel,
                                                 const NRSC5::DataService &component,
                                                 const NRSC5::Lot &lot)
{
	emit controller_->HDReceivedLot(channel, component, lot);
}
