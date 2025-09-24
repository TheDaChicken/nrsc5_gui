//
// Created by TheDaChicken on 9/2/2025.
//

#ifndef SAMPLEFORMAT_H
#define SAMPLEFORMAT_H

namespace AUDIO
{
enum SampleFormat
{
	Uint8,
	Int16,
	Int24,
	Int32,
	Float32,
};

inline int GetSampleSize(const SampleFormat fmt)
{
	switch (fmt)
	{
		case Uint8: return 1;
		case Int16: return 2;
		case Int24: return 3;
		case Int32: return 4;
		case Float32: return 4;
		default: return 0;
	}
}
}

#endif //SAMPLEFORMAT_H
