//
// Created by TheDaChicken on 9/5/2023.
//

#include "Stream.h"

PortAudio::Stream::Stream()
  : m_stream(nullptr)
{
}

PortAudio::Stream::~Stream()
{
  Close();
}

bool PortAudio::Stream::IsOpen() const
{
  return m_stream != nullptr;
}

int PortAudio::Stream::Start()
{
  return Pa_StartStream(m_stream);
}

int PortAudio::Stream::Stop()
{
  return Pa_StopStream(m_stream);
}

int PortAudio::Stream::Abort()
{
  return Pa_AbortStream(m_stream);
}

bool PortAudio::Stream::IsStopped() const
{
  return Pa_IsStreamStopped(m_stream) == 1;
}

bool PortAudio::Stream::IsActive() const
{
  return Pa_IsStreamActive(m_stream) == 1;
}

double PortAudio::Stream::GetSampleRate() const
{
  const PaStreamInfo *info = Pa_GetStreamInfo(m_stream);
  if (info == nullptr)
    return paInternalError;

  return info->sampleRate;
}

double PortAudio::Stream::GetOutputLatency() const
{
  const PaStreamInfo *info = Pa_GetStreamInfo(m_stream);
  if (info == nullptr)
    return paInternalError;

  return info->outputLatency;
}

PaTime PortAudio::Stream::GetTime() const
{
  return Pa_GetStreamTime(m_stream);
}

void PortAudio::Stream::Close()
{
  Pa_CloseStream(m_stream);
  m_stream = nullptr;
}
