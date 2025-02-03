//
// Created by TheDaChicken on 9/5/2023.
//

#ifndef NRSC5_GUI_STREAMPARAMETERS_H
#define NRSC5_GUI_STREAMPARAMETERS_H

#include "StreamParametersDirectionSpecific.h"

#define FRAMES_PER_BUFFER   (64)

namespace PortAudio
{
struct StreamParameters
{
	// Half-duplex specific parameters:
	StreamParametersDirectionSpecific inputParameters_;
	StreamParametersDirectionSpecific outputParameters_;

	double sampleRate{0};
	unsigned long framesPerBuffer_{FRAMES_PER_BUFFER};
	int flags{0};

	bool IsSupported() const;
};
}

#endif //NRSC5_GUI_STREAMPARAMETERS_H
