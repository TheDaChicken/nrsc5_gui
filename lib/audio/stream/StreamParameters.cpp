//
// Created by TheDaChicken on 9/5/2023.
//

#include "StreamParameters.h"

bool PortAudio::StreamParameters::IsSupported() const
{
	return Pa_IsFormatSupported(&inputParameters_,
	                            &outputParameters_,
	                            sampleRate) == paFormatIsSupported;
}
