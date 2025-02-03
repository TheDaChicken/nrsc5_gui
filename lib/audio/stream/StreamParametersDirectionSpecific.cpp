//
// Created by TheDaChicken on 9/5/2023.
//

#include "StreamParametersDirectionSpecific.h"

PortAudio::StreamParametersDirectionSpecific::StreamParametersDirectionSpecific(
  const std::shared_ptr<Device> &device, int numChannels) : PaStreamParameters()
{
  SetDevice(device);
  channelCount = numChannels;
}

void PortAudio::StreamParametersDirectionSpecific::SetDevice(const std::shared_ptr<Device> &device)
{
  this->device = device->GetIndex();
}
