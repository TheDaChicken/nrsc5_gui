//
// Created by TheDaChicken on 9/5/2023.
//

#ifndef NRSC5_GUI_RINGBUFFERSTREAM_H
#define NRSC5_GUI_RINGBUFFERSTREAM_H

#include <atomic>

#include "StreamBuffer.h"
#include "audio/stream/Stream.h"
#include "audio/stream/StreamParameters.h"

extern "C" {
#include "portaudio.h"
}

namespace PortAudio
{
struct StreamParametersRingBuffer : StreamParameters
{
	StreamParametersRingBuffer() = default;
	explicit StreamParametersRingBuffer(const StreamParameters &parameters)
		: StreamParameters(parameters)
	{
	}

	size_t bufferSize{0}; // Extra buffer size
	size_t bufferLatency{0}; // Startup delay amount
};

/**
 * @brief StreamOutputPush class for PortAudio audio output stream for live audio data
 */
class StreamLiveOutputPush : public Stream
{
	public:
		StreamLiveOutputPush();
		~StreamLiveOutputPush() = default;

		/**
		 * @brief Open the stream
		 * @param parameters Parameters for the stream
		 * @return status code from PortAudio
		 */
		int Open(const StreamParametersRingBuffer &parameters);

		/**
		 * @brief Time of written data in the stream
		 * @return The time
		 */
		[[nodiscard]] PaTime GetWrittenTime() const
		{
			return ring_buffer.GetWrittenTime();
		}

		/**
		 * @brief Reset the stream
		 */
		void Reset();

		int WriteFrame(const void *buffer, unsigned int sample_size);

		int Stop();
	private:
		std::size_t GenerateBufferSize() const;

		static int paCallbackFun(
			const void *inputBuffer, void *outputBuffer,
			unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
			PaStreamCallbackFlags statusFlags, void *userData);

		std::size_t element_size;
		StreamBuffer ring_buffer;
};
}

#endif //NRSC5_GUI_RINGBUFFERSTREAM_H
