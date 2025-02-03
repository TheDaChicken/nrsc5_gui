//
// Created by TheDaChicken on 8/6/2024.
//

#ifndef NRSC5_GUI_SRC_THREADS_GUIEVENT_H_
#define NRSC5_GUI_SRC_THREADS_GUIEVENT_H_

#include <utility>

#include "RadioChannel.h"
#include "portaudio.h"
#include "nrsc5/Station.h"
#include "utils/MessageQueue.h"

#include <QPromise>

class GuiMessage : public QueueMessage
{
 public:
  enum EventType
  {
	NONE = 1,

	// HDRadio events
	EVENT_HD_SYNC = 2,
	EVENT_HD_STATION = 3,
	EVENT_HD_ID3 = 4,
	EVENT_HD_LOT = 5,
  };

  explicit GuiMessage(EventType type)
	  : QueueMessage(QueueMessage::GENERAL_GUI_UPDATE), m_event(type)
  {
  }

  [[nodiscard]]
  inline bool IsType(EventType type) const
  {
	return m_event == type;
  }

  [[nodiscard]]
  inline EventType GetEventType() const
  {
	return m_event;
  }

 private:
  EventType m_event;
};

struct GuiSyncEvent : public GuiMessage
{
  explicit GuiSyncEvent(GuiMessage::EventType type, PaTime time) : GuiMessage(type), time_(time)
  {
  }

  PaTime time_;
};

struct GuiHDSyncEvent : public GuiSyncEvent
{
  explicit GuiHDSyncEvent(PaTime time, bool on = true)
	  : GuiSyncEvent(GuiMessage::EVENT_HD_SYNC, time), on_(on)
  {

  }

  bool on_;
};

struct GuiStationUpdate : public GuiSyncEvent
{
  explicit GuiStationUpdate(PaTime time, const RadioChannel &channel)
	  : GuiSyncEvent(GuiMessage::EVENT_HD_STATION, time), channel_(channel)
  {

  }

  RadioChannel channel_;
};

struct GuiID3Update : public GuiSyncEvent
{
  explicit GuiID3Update(PaTime time, NRSC5::ID3 id3)
	  : GuiSyncEvent(GuiMessage::EVENT_HD_ID3, time), id3_(std::move(id3))
  {

  }

  NRSC5::ID3 id3_;
};

#endif //NRSC5_GUI_SRC_THREADS_GUIEVENT_H_
