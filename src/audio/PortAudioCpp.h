//
// Created by TheDaChicken on 9/5/2023.
//

#ifndef NRSC5_GUI_SYSTEM_H
#define NRSC5_GUI_SYSTEM_H

#include "audio/Device.h"
#include "audio/HostApi.h"

#include <vector>
#include <memory>

extern "C" {
#include "portaudio.h"
}

namespace PortAudio
{
class System : public std::enable_shared_from_this<System>
{
 public:
  System();
  ~System();

  static std::string_view VersionText();
  static std::string_view ErrorText(int error);

  PaError Initialize();

  int DefaultInputDevice(std::shared_ptr<Device> &device);
  int DefaultOutputDevice(std::shared_ptr<Device> &device);
  int DefaultHostApi(std::shared_ptr<HostApi> &host_api);

  int DeviceByIndex(PaDeviceIndex index, std::shared_ptr<Device> &device);
  int HostApiByIndex(PaHostApiIndex index, std::shared_ptr<HostApi> &host);

  [[nodiscard]] const std::vector<std::shared_ptr<HostApi> > &HostApis() const;
  [[nodiscard]] const std::vector<std::shared_ptr<Device> > &Devices() const;

  std::shared_ptr<Device> &NullDevice();

  int DeviceCount()
  {
   return m_devices.size();
  }
  int HostApiCount()
  {
   return m_hostApis.size();
  }

 private:
  std::shared_ptr<Device> m_nullDevice;
  std::vector<std::shared_ptr<Device> > m_devices;
  std::vector<std::shared_ptr<HostApi> > m_hostApis;
};
}

#endif //NRSC5_GUI_SYSTEM_H
