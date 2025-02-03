//
// Created by TheDaChicken on 9/5/2023.
//

#ifndef NRSC5_GUI_HOSTAPI_H
#define NRSC5_GUI_HOSTAPI_H

#include "audio/PortAudioCpp.h"
#include "audio/Device.h"

#include <string_view>
#include <vector>
#include <memory>

extern "C" {
#include "portaudio.h"
}

namespace PortAudio
{
class Device;
}

namespace PortAudio
{
class HostApi
{
 public:
  explicit HostApi(const std::weak_ptr<System> &system, PaHostApiIndex index);
  ~HostApi() = default;

  [[nodiscard]] std::string_view Name() const;
  [[nodiscard]] int DeviceCount() const;

  std::vector<std::shared_ptr<Device> > &Devices();

 private:
  const PaHostApiInfo *m_info;
  PaHostApiIndex m_index;
  std::vector<std::shared_ptr<Device> > m_devices;
};
}

#endif //NRSC5_GUI_HOSTAPI_H
