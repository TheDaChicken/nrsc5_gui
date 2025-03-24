//
// Created by TheDaChicken on 9/5/2023.
//

#include "PortAudioCpp.h"

#include "Device.h"
#include "HostApi.h"

#include "utils/Log.h"

#include <portaudio.h>

#include <memory>

std::string_view PortAudio::System::VersionText()
{
  return Pa_GetVersionText();
}

PortAudio::System::System() = default;

PaError PortAudio::System::Initialize()
{
  PaError ret = Pa_Initialize();
  if (ret < paNoError)
  {
    Logger::Log(err, "Failed to initialize PortAudio: {}", Pa_GetErrorText(ret));
    return ret;
  }

  const int deviceCount = Pa_GetDeviceCount();
  if (deviceCount < 0)
  {
    Logger::Log(err, "Failed to get device count: {}", Pa_GetErrorText(deviceCount));
    return deviceCount;
  }

  const int hostApiCount = Pa_GetHostApiCount();
  if (hostApiCount < 0)
  {
    Logger::Log(err, "Failed to get host API count: {}", Pa_GetErrorText(hostApiCount));
    return hostApiCount;
  }

  // Initialize devices
  m_devices.resize(deviceCount);
  for (PaDeviceIndex i = 0; i < deviceCount; i++)
  {
    m_devices[i] = std::make_shared<Device>(shared_from_this(), i);
  }

  // Initialize host APIs
  m_hostApis.resize(hostApiCount);
  for (PaHostApiIndex i = 0; i < hostApiCount; i++)
  {
    m_hostApis[i] = std::make_shared<HostApi>(shared_from_this(), i);
  }

  // Initialize null device
  m_nullDevice = std::make_shared<Device>(shared_from_this(), paNoDevice);
  return paNoError;
}

std::string_view PortAudio::System::ErrorText(int error)
{
  return Pa_GetErrorText(error);
}

PortAudio::System::~System()
{
  m_hostApis.clear();
  m_devices.clear();

  Pa_Terminate();
}

int PortAudio::System::DefaultOutputDevice(std::shared_ptr<Device> &device)
{
  const PaDeviceIndex index = Pa_GetDefaultOutputDevice();
  return DeviceByIndex(index, device);
}

int PortAudio::System::DefaultInputDevice(std::shared_ptr<Device> &device)
{
  const PaDeviceIndex index = Pa_GetDefaultInputDevice();
  return DeviceByIndex(index, device);
}

int PortAudio::System::DefaultHostApi(std::shared_ptr<HostApi> &host_api)
{
  const PaHostApiIndex index = Pa_GetDefaultHostApi();
  if (index < paNoError)
  {
    return index;
  }

  host_api = m_hostApis[index];
  return paNoError;
}

int PortAudio::System::DeviceByIndex(PaDeviceIndex index, std::shared_ptr<Device> &device)
{
  if (index < -1 || index >= DeviceCount())
  {
    return paInternalError;
  }
  if (index == -1)
  {
    device = m_nullDevice;
  }
  else
  {
    device = m_devices[index];
  }
  return paNoError;
}

int PortAudio::System::HostApiByIndex(PaHostApiIndex index, std::shared_ptr<HostApi> &host)
{
  if (index >= HostApiCount())
  {
    return paInternalError;
  }

  host = m_hostApis[index];
  return paNoError;
}

std::shared_ptr<PortAudio::Device> &PortAudio::System::NullDevice()
{
  return m_nullDevice;
}

const std::vector<std::shared_ptr<PortAudio::Device> > &PortAudio::System::Devices() const
{
  return m_devices;
}

const std::vector<std::shared_ptr<PortAudio::HostApi> > &PortAudio::System::HostApis() const
{
  return m_hostApis;
}
