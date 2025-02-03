//
// Created by TheDaChicken on 9/5/2023.
//

#include "StreamOutputPush.h"

#include "audio/stream/StreamParameters.h"
#include "utils/Log.h"

PortAudio::StreamLiveOutputPush::StreamLiveOutputPush()
	: element_size(0)
{
}

int PortAudio::StreamLiveOutputPush::paCallbackFun(
	const void *input_buffer, void *output_buffer,
	unsigned long frames_per_buffer,
	const PaStreamCallbackTimeInfo *time_info,
	PaStreamCallbackFlags status_flags,
	void *userData)
{
	auto *data = static_cast<StreamLiveOutputPush *>(userData);
	const_cast<void *>(input_buffer); /* Prevent unused variable warning. */

	if (data->ring_buffer.Read(output_buffer, frames_per_buffer, time_info->outputBufferDacTime) < 0)
	{
		Logger::Log(err, "PortAudio: Failed to read from ring buffer");
		return paAbort;
	}
	return paContinue;
}

int PortAudio::StreamLiveOutputPush::Open(
	const StreamParametersRingBuffer &parameters)
{
	const PaStreamParameters output_parameters = parameters.outputParameters_;

	element_size = Pa_GetSampleSize(output_parameters.sampleFormat) * output_parameters.channelCount;

	int ret = Pa_OpenStream(&m_stream,
	                        nullptr,
	                        &output_parameters,
	                        parameters.sampleRate,
	                        parameters.framesPerBuffer_,
	                        parameters.flags,
	                        &paCallbackFun,
	                        this);
	if (ret != paNoError)
	{
		Logger::Log(err, "PortAudio: Failed to open stream: {}", Pa_GetErrorText(ret));
		return ret;
	}

	ring_buffer.Initialize(parameters.bufferSize,
	                       parameters.bufferLatency + parameters.framesPerBuffer_,
	                       output_parameters.suggestedLatency,
	                       element_size,
	                       GetSampleRate());
	return 0;
}

void PortAudio::StreamLiveOutputPush::Reset()
{
	if (!m_stream || !IsActive())
		return;

	Abort();
	ring_buffer.Reset();
	Start();
}

int PortAudio::StreamLiveOutputPush::Stop()
{
	if (!m_stream)
		return -1;

	int ret = Pa_StopStream(m_stream);
	if (ret != paNoError)
	{
		Logger::Log(err, "PortAudio: Failed to stop stream: {}", Pa_GetErrorText(ret));
		return ret;
	}

	ring_buffer.Reset();
	return 0;
}

int PortAudio::StreamLiveOutputPush::WriteFrame(const void *buffer, unsigned int sample_size)
{
	if (!m_stream)
		return -1;

	return ring_buffer.Write(buffer, sample_size);
}
