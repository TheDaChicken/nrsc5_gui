//
// Created by TheDaChicken on 8/6/2024.
//

#ifndef NRSC5_GUI_SRC_THREADS_GUISYNCTHREAD_H_
#define NRSC5_GUI_SRC_THREADS_GUISYNCTHREAD_H_

#include "GuiEvent.h"
#include "utils/MessageQueue.h"
#include "audio/stream/Stream.h"

#include <QThread>

class GuiSyncThread : public QThread
{
  Q_OBJECT

 public:
  GuiSyncThread();

  void Put(std::unique_ptr<GuiSyncEvent> &&event)
  {
   m_queue.Push(std::move(event));
  }

  void SetStream(const PortAudio::Stream *stream)
  {
   assert(stream);
   m_stream = stream;
  }

  void Stop();
  void run() override;
 signals:
  void SyncEvent(const std::shared_ptr<GuiSyncEvent> &event);

 private:
  const PortAudio::Stream *m_stream = nullptr;

  std::atomic<bool> m_stop = false;
  MessageQueue m_queue;

  static int SleepTime(const PaTime &start_time, const PaTime &current_time);
};

#endif //NRSC5_GUI_SRC_THREADS_GUISYNCTHREAD_H_
