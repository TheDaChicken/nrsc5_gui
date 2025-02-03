//
// Created by TheDaChicken on 9/5/2023.
//

#include "audio/Device.h"

#include "audio/PortAudioCpp.h"
#include "audio/HostApi.h"

PortAudio::Device::Device(const std::weak_ptr<System> &system, PaDeviceIndex index)
  : m_system(system), m_index(index)
{
  if (index == paNoDevice)
    m_info = nullptr;
  else
    m_info = Pa_GetDeviceInfo(index);
}

std::string_view PortAudio::Device::Name() const
{
  if (m_info == nullptr)
    return "";

  return m_info->name;
}

PaTime PortAudio::Device::DefaultLowInputLatency() const
{
  if (m_info == nullptr)
    return 0;

  return m_info->defaultLowInputLatency;
}

PaTime PortAudio::Device::DefaultLowOutputLatency() const
{
  if (m_info == nullptr)
    return 0;

  return m_info->defaultLowOutputLatency;
}

PaTime PortAudio::Device::DefaultHighOutputLatency() const
{
  if (m_info == nullptr)
    return 0;

  return m_info->defaultHighOutputLatency;
}

int PortAudio::Device::MaxInputChannels() const
{
  if (m_info == nullptr)
    return 0;

  return m_info->maxInputChannels;
}

int PortAudio::Device::MaxOutputChannels() const
{
  if (m_info == nullptr)
    return 0;

  return m_info->maxOutputChannels;
}

bool PortAudio::Device::IsInputOnlyDevice() const
{
  return MaxOutputChannels() == 0;
}

bool PortAudio::Device::IsOutputOnlyDevice() const
{
  return MaxInputChannels() == 0;
}

bool PortAudio::Device::operator==(const Device &rhs) const
{
  return m_index == rhs.m_index;
}

int PortAudio::Device::HostApi(std::shared_ptr<PortAudio::HostApi> &host_api)
{
  if (m_info == nullptr)
    return paInternalError;

  if (auto system = m_system.lock())
  {
    return system->HostApiByIndex(m_info->hostApi, host_api);
  }

  return paInternalError;
}

int PortAudio::Device::HostApi(std::shared_ptr<PortAudio::HostApi> &host_api) const
{
  if (m_info == nullptr)
    return paInternalError;

  if (auto system = m_system.lock())
  {
    return system->HostApiByIndex(m_info->hostApi, host_api);
  }

  return paInternalError;
}
