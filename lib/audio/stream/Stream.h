//
// Created by TheDaChicken on 9/5/2023.
//

#ifndef NRSC5_GUI_PORTAUDIO_STREAM_H
#define NRSC5_GUI_PORTAUDIO_STREAM_H

extern "C"
{
#include "portaudio.h"
}

namespace PortAudio
{
class Stream
{
 public:
  Stream();
  ~Stream();

  [[nodiscard]] bool IsOpen() const;

  int Start();
  int Stop();

  int Abort();
  void Close();

  [[nodiscard]] PaTime GetTime() const;
  [[nodiscard]] double GetSampleRate() const;
  [[nodiscard]] double GetOutputLatency() const;

  [[nodiscard]] bool IsStopped() const;
  [[nodiscard]] bool IsActive() const;
 protected:
  PaStream *m_stream;
};
}

#endif //NRSC5_GUI_PORTAUDIO_STREAM_H
