//
// Created by TheDaChicken on 9/5/2023.
//

#ifndef NRSC5_GUI_AUDIO_DEVICE_H
#define NRSC5_GUI_AUDIO_DEVICE_H

#include <memory>
#include <string_view>

extern "C" {
#include "portaudio.h"
}

namespace PortAudio
{
class System;
class HostApi;
class Device
{
 public:
  explicit Device(const std::weak_ptr<System> &system, PaDeviceIndex index);

  ~Device() = default;

  [[nodiscard]] int GetIndex() const
  {
   return m_index;
  };
  [[nodiscard]] std::string_view Name() const;
  [[nodiscard]] PaTime DefaultLowInputLatency() const;
  [[nodiscard]] PaTime DefaultLowOutputLatency() const;
  [[nodiscard]] PaTime DefaultHighOutputLatency() const;

  [[nodiscard]] bool IsInputOnlyDevice() const;
  [[nodiscard]] bool IsOutputOnlyDevice() const;

  [[nodiscard]] int MaxOutputChannels() const;
  [[nodiscard]] int MaxInputChannels() const;

  // host api reference
  int HostApi(std::shared_ptr<HostApi> &host_api);
  [[nodiscard]] int HostApi(std::shared_ptr<PortAudio::HostApi> &) const;

  bool operator==(const Device &rhs) const;

  bool isNull() const
  {
   return m_index == -1;
  }

 System *GetSystem()
  {
   if (auto system = m_system.lock())
   {
    return system.get();
   }
   return nullptr;
  }

 private:
  const std::weak_ptr<System> &m_system;

  PaDeviceIndex m_index;
  const PaDeviceInfo *m_info;
};
}

#endif //NRSC5_GUI_AUDIO_DEVICE_H
