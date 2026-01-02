//
// Created by TheDaChicken on 11/22/2025.
//

#include "SDRController.h"

#include "utils/Log.h"

SDRController::SDRController()
{
	host_.OnNewDevice = [this](const SDRInfo &dev)
	{
		// DeviceUi ui;
		//
		// ui.id = dev;
		// ui.name = dev.name;
		//
		// devices_.emplace_back(ui);
		//
		// Logger::Log(info, "SDRController: Added SDR device: {}", dev.name);
	};

	host_.OnDeviceRemoved = [this](const SDRInfo &device_info)
	{
		// for (auto iter = devices_.begin(); iter != devices_.end(); )
		// {
		// 	if (device_info.name == iter->name)
		// 		iter = devices_.erase(iter);
		// 	else
		// 		++iter;
		// }
		//
		// Logger::Log(info, "SDRController: Removed SDR device: {}", device_info.name);
	};
}

SDRController::~SDRController()
{

}

// UTILS::Future<std::vector<SDRInfo>, int> SDRController::GetDevices()
// {
// 	UTILS::Promise<std::vector<SDRInfo>, int> promise;
// 	UTILS::Future<std::vector<SDRInfo>, int> future = promise.GetFuture();
//
// 	UTILS::ThreadPool::GetInstance().QueueJob([this, p = std::move(promise)]() mutable
// 	{
// 		if (p.IsFinished())
// 			return;
//
// 		auto devices = host_.GetDevices();
// 		p.SetValue(std::move(devices));
// 	});
//
// 	return future;
// }

std::shared_ptr<HybridSession> SDRController::OpenSDR(
	const SDRInfo &device)
{
	Logger::Log(info, "Opening device: {}", device.serial);

	UTILS::ThreadPool::GetInstance().QueueJob([this, device]()
	{
		auto result = host_.CreateSession(device);
		if (!result)
			return;

		sessions_[device].PushInput(std::move(result));
	});
}

// void SDRController::CloseSDR(SessionContext &session)
// {
// 	// ThreadPool::GetInstance().QueueJob([this, session_id, promise]
// 	// {
// 	// 	if (promise->IsFinished())
// 	// 		return;
// 	//
// 	// 	sessions_.DeleteSession(session_id);
// 	//
// 	// 	promise->SetValue();
// 	// });
// 	//
// 	// session.closing_device_future = promise->GetFuture();
// }


// void SDRController::StartSession(
// 	SessionContext &context)
// {
// 	const auto ret = context.session.Start();
// 	if (ret == INPUT_OK)
// 	{
// 		//context.running = true;
// 	}
//
// 	Logger::Log(info,
// 	            "Starting {} ret: {}",
// 	            *context.session_id_,
// 	            static_cast<int>(ret));
// }
//
// void SDRController::StopSession(
// 	SessionContext &context)
// {
// 	const auto ret = context.session.Stop();
// 	if (ret == INPUT_OK)
// 	{
// 		//context.running = false;
// 	}
//
// 	Logger::Log(info, "Stopping {} ret: {}", *context.session_id_, static_cast<int>(ret));
// }
//
// void SDRController::UpdateGain(
// 	SessionContext &context,
// 	const std::string &name,
// 	const float gain)
// {
// 	Logger::Log(info, "Changing gain {}", gain);
//
// 	if (const auto sdr = context.session.GetSDRControl())
// 	{
// 		sdr->SetGain(name, gain);
// 	}
// }
//
// void SDRController::UpdateGainMode(
// 	SessionContext &context,
// 	const PortSDR::GainMode mode)
// {
// 	if (const auto sdr = context.session.GetSDRControl())
// 	{
// 		Logger::Log(info, "Changing gain mode {}", static_cast<int>(mode));
//
// 		sdr->SetGainMode(mode);
// 	}
// }
//
// void SDRController::SetFrequency(
// 	SessionContext &context,
// 	const Band::Type type,
// 	const uint32_t freq)
// {
// 	context.session.SetFrequency(type, freq);
// }
