//
// Created by TheDaChicken on 12/18/2025.
//

#include "SDRHost.h"

#include <unordered_set>

template <>
struct std::hash<SDRInfo>
{
	std::size_t operator()(const SDRInfo& k) const
	{
		return std::hash<std::string>()(k.serial)
				^ (std::hash<int>()(k.type) << 1);
	}
};

void SDRHost::UpdateDeviceList()
{
	const std::vector<PortSDR::Device> devices = sdr_.GetDevices();
	std::unique_ptr<PortSDR::Stream> stream;
	std::unordered_set<SDRInfo> seen;
	PortSDR::DeviceInfo info;

	std::scoped_lock lock(mutex_);

	for (const auto &dev : devices)
	{
		const auto host = dev.host.lock();
		if (!host) continue;

		SDRInfo key
		{
			.serial = dev.serial,
			.type = host->GetType(),
		};

		seen.insert(key);

		if (auto iter = devices_.find(key); iter == devices_.end())
		{
			AddDevice(dev);
		}
	}

	for (auto it = devices_.begin(); it != devices_.end();)
	{
		if (auto iter = seen.find(it->first); iter == seen.end() && it->second.dev.expired())
		{
			if (OnDeviceRemoved)
				OnDeviceRemoved(it->first);
			it = devices_.erase(it);
		}
		else
			++it;
	}
}

std::shared_ptr<IRadioInput> SDRHost::CreateSession(const SDRInfo &device)
{
	std::scoped_lock lock(mutex_);

	auto iter = devices_.find(device);
	if (iter == devices_.end())
		return nullptr;

	auto& [_, info] = *iter;

	if (info.dev.expired())
	{
		std::unique_ptr<PortSDR::Stream> stream;

		auto err = info.device.CreateStream(stream);
		if (err != PortSDR::ErrorCode::OK)
			return nullptr;

		auto input = std::make_shared<SDRInput>(std::move(stream));
		info.dev = input;
		return input;
	}

	return {};
}

void SDRHost::AddDevice(const PortSDR::Device &device)
{
	std::unique_ptr<PortSDR::Stream> stream;

	auto ret = device.CreateStream(stream);
	if (ret != PortSDR::ErrorCode::OK)
		return;

	auto strings = stream->GetUSBStrings();

	SDRInfo key
	{
		.name = strings.name,
		.serial = strings.serial,
		.type = device.host.lock()->GetType(),
	};

	devices_.emplace(
		key,
		DeviceHolder{
			.device = device
		}
	);

	if (OnNewDevice)
		OnNewDevice(devices_.find(key)->first);
}

