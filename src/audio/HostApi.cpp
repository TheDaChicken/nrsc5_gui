//
// Created by TheDaChicken on 9/5/2023.
//

#include "audio/HostApi.h"

#include "audio/PortAudioCpp.h"
#include "audio/Device.h"

PortAudio::HostApi::HostApi(const std::weak_ptr<System> &system_ptr, PaHostApiIndex index)
	: m_index(index)
{
	if (index == paNoDevice)
		m_info = nullptr;
	else
		m_info = Pa_GetHostApiInfo(index);

	// Create and populate devices array:
	const int num_devices = DeviceCount();

	m_devices.resize(num_devices);

	if (auto system = system_ptr.lock())
	{
		for (PaDeviceIndex i = 0; i < num_devices; i++)
		{
			PaDeviceIndex deviceIndex = Pa_HostApiDeviceIndexToDeviceIndex(index, i);
			system->DeviceByIndex(deviceIndex, m_devices[i]);
		}
	}
}

int PortAudio::HostApi::DeviceCount() const
{
	if (m_info == nullptr)
		return 0;

	return m_info->deviceCount;
}

std::string_view PortAudio::HostApi::Name() const
{
	if (m_info == nullptr)
		return "";

	return m_info->name;
}

std::vector<std::shared_ptr<PortAudio::Device> > &PortAudio::HostApi::Devices()
{
	return m_devices;
}
