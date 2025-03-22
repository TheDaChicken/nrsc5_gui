//
// Created by TheDaChicken on 8/6/2024.
//

#ifndef NRSC5_GUI_SRC_BACKEND_UTILS_MESSAGEQUEUE_H_
#define NRSC5_GUI_SRC_BACKEND_UTILS_MESSAGEQUEUE_H_

#include <list>
#include <memory>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <QUuid>

class QueueMessage
{
 public:
  enum MessageType
  {
	NONE = 1,
	GENERAL_GUI_UPDATE
  };

  explicit QueueMessage(MessageType msg)
  {
	m_message = msg;
  }

  virtual ~QueueMessage() = default;

  [[nodiscard]]
  inline bool IsType(MessageType msg) const
  {
	return m_message == msg;
  }

  [[nodiscard]]
  inline MessageType GetMessageType() const
  {
	return m_message;
  }

  inline QString GetId() const
  {
	return id;
  }

 private:
  MessageType m_message;
  QString id = QUuid::createUuid().toString();
};

class MessageQueue
{
 public:
  explicit MessageQueue(int MAX_QUEUE_SIZE = 100);
  ~MessageQueue();

  void Flush(QueueMessage::MessageType type = QueueMessage::NONE);
  void Close();

  int Push(std::unique_ptr<QueueMessage> &&msg);
  bool Pop(std::unique_ptr<QueueMessage> &msg, const std::chrono::milliseconds &timeout = std::chrono::milliseconds::zero());

  [[nodiscard]]
  inline bool IsEmpty() const
  {
	std::lock_guard<std::mutex> lock(mutex_);
	return queue_.empty();
  }

  std::optional<std::reference_wrapper<std::unique_ptr<QueueMessage>>> Find(const QString &id);

 private:
  mutable std::mutex mutex_;
  int queueSize_;

  std::condition_variable event_;
  std::list<std::unique_ptr<QueueMessage>> queue_;
};

#endif //NRSC5_GUI_SRC_BACKEND_UTILS_MESSAGEQUEUE_H_
