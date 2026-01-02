//
// Created by TheDaChicken on 12/18/2025.
//

#ifndef NRSC5_GUI_SDRAPI_H
#define NRSC5_GUI_SDRAPI_H

#include "input/SDRInput.h"

using DeviceId = uint32_t;

struct DeviceHolder
{
	PortSDR::Device device;
	std::weak_ptr<SDRInput> dev;
};

struct SDRInfo
{
	const std::string name;
	const std::string serial;
	const PortSDR::Host::HostType type;

	bool operator==(const SDRInfo &o) const
	{
		return serial == o.serial && type == o.type;
	}

	bool operator<(const SDRInfo &o) const
	{
		return serial < o.serial && type < o.type;
	}
};

class SDRHost
{
	public:
		std::vector<SDRInfo> GetDeviceList()
		{
			std::scoped_lock lock(mutex_);
			std::vector<SDRInfo> devices;
			for (auto&[fst, snd] : devices_)
				devices.push_back(fst);
			return devices;
		}

		/**
		 * Creates a session for a device.
		 * This can be only created once.
		 * The next time will return a null.
		 *  TODO: Add error code.
		 * @param device
		 * @return
		 */
		std::shared_ptr<IRadioInput> CreateSession(const SDRInfo& device);

		void UpdateDeviceList();

		std::function<void(const SDRInfo&)> OnNewDevice;
		std::function<void(const SDRInfo&)> OnDeviceRemoved;
	private:
		/**
		 * Adds to SDR cached devices.
		 * This has no mutex lock so it's private :)
		 * @param device device to add.
		 */
		void AddDevice(const PortSDR::Device &device);

		PortSDR::PortSDR sdr_;
		std::map<SDRInfo, DeviceHolder> devices_;
		mutable std::mutex mutex_;
};

#endif //NRSC5_GUI_SDRAPI_H
