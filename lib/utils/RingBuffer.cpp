//
// Created by TheDaChicken on 5/4/2024.
//

#include "RingBuffer.h"
#include "Log.h"

#include <cstring>

RingBuffer::RingBuffer()
	: m_unitSize(0), m_size(0), m_readCount(0), m_writeCount(0)
{

}

RingBuffer::~RingBuffer()
{
  Close();
}

void RingBuffer::Close()
{
	m_data.clear();
}

void RingBuffer::Open(const size_t size, const size_t unit_size)
{
  m_data.resize(size * unit_size);
  m_size = size;
  m_unitSize = unit_size;
}

void RingBuffer::Flush()
{
  m_writeCount.store(0, std::memory_order_release);
  m_readCount.store(0, std::memory_order_release);
}

size_t RingBuffer::Write(const void *data, const size_t size)
{
  /* Preload variables with adequate memory ordering */
  size_t write = m_writeCount.load(std::memory_order_relaxed);
  size_t cnt = size;
  const size_t read = m_readCount.load(std::memory_order_acquire);
  const size_t free = CalcFree(write, read);

  if (free == 0)
  {
	return 0;
  }

  if (free < cnt)
  {
	cnt = free;
  }

  /* CheckChannelStatus if the write wraps */
  if (write + cnt <= m_size)
  {
	/* Copy in the linear region */
	memcpy(&m_data[write * m_unitSize], data, cnt * m_unitSize);
	/* Correct the write index */
	write = (write + cnt) % m_size;
  }
  else
  {
	/* Copy in the linear region */
	const size_t linear_free = m_size - write;
	memcpy(&m_data[write * m_unitSize], data, linear_free * m_unitSize);
	/* Copy remaining to the beginning of the buffer */
	const size_t remaining = cnt - linear_free;
	memcpy(&m_data[0], static_cast<const std::byte *>(data) + (linear_free * m_unitSize), remaining * m_unitSize);
	/* Correct the write index */
	write = remaining;
  }

  /* Store the write index with adequate ordering */
  m_writeCount.store(write, std::memory_order_release);

  return cnt;
}

size_t RingBuffer::Read(void *data, const size_t size)
{
  /* Preload variables with adequate memory ordering */
  size_t read = m_readCount.load(std::memory_order_relaxed);
  size_t cnt = size;
  const size_t write = m_writeCount.load(std::memory_order_acquire);
  const size_t free = CalcAvailable(write, read);

  if (free == 0)
  {
	return 0;
  }

  if (free < cnt)
  {
	cnt = free;
  }

  /* CheckChannelStatus if the read wraps */
  if (read + cnt <= m_size)
  {
	/* Copy in the linear region */
	memcpy(data, &m_data[read * m_unitSize], cnt * m_unitSize);
	/* Correct the read index */
	read = (read + cnt) % m_size;
  }
  else
  {
	/* Copy in the linear region */
	const size_t linear_available = m_size - read;
	memcpy(data, &m_data[read * m_unitSize], linear_available * m_unitSize);
	/* Copy remaining from the beginning of the buffer */
	const size_t remaining = cnt - linear_available;
	memcpy(static_cast<std::byte *>(data) + (linear_available * m_unitSize),
		   &m_data[0], remaining * m_unitSize);
	/* Correct the read index */
	read = remaining;
  }

  /* Store the write index with adequate ordering */
  m_readCount.store(read, std::memory_order_release);

  return cnt;
}

size_t RingBuffer::GetFree() const
{
  const size_t w = m_writeCount.load(std::memory_order_relaxed);
  const size_t r = m_readCount.load(std::memory_order_acquire);

  return CalcFree(w, r);
}

size_t RingBuffer::GetAvailable() const
{
  const size_t r = m_readCount.load(std::memory_order_relaxed);
  const size_t w = m_writeCount.load(std::memory_order_acquire);

  return CalcAvailable(w, r);
}

size_t RingBuffer::CalcFree(const size_t w, const size_t r) const
{
  if (r > w)
  {
	return (r - w) - 1U;
  }
  return (m_size - (w - r)) - 1U;
}

size_t RingBuffer::CalcAvailable(const size_t w, const size_t r) const
{
  if (w >= r)
  {
	return w - r;
  }
  else
  {
	return m_size - (r - w);
  }
}
