#ifndef NRSC5_GUI_DIRECTIONSPECIFICSTREAMPARAMETERS_H
#define NRSC5_GUI_DIRECTIONSPECIFICSTREAMPARAMETERS_H

#include "audio/SampleDataFormat.h"
#include "audio/Device.h"

extern "C" {
#include "portaudio.h"
}

namespace PortAudio
{
class StreamParametersDirectionSpecific : public PaStreamParameters
{
	public:
		StreamParametersDirectionSpecific() : PaStreamParameters()
		{
		}
		StreamParametersDirectionSpecific(const std::shared_ptr<Device> &device, int numChannels);

		void SetDevice(const std::shared_ptr<Device> &device);
};
}

#endif //NRSC5_GUI_DIRECTIONSPECIFICSTREAMPARAMETERS_H
