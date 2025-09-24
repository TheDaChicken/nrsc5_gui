//
// Created by TheDaChicken on 5/4/2024.
//

#include "RingBuffer.h"
#include "Log.h"

#include <cstring>

RingBuffer::RingBuffer()
	: elementSize_(0), size_(0), m_readCount(0), m_writeCount(0)
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

void RingBuffer::Open(const size_t size, const size_t element_size)
{
	m_data.resize(size * element_size);
	size_ = size;
	elementSize_ = element_size;
}

void RingBuffer::Flush()
{
	m_writeCount.store(0, std::memory_order_release);
	m_readCount.store(0, std::memory_order_release);
}

size_t RingBuffer::Write(const void *data, const size_t size)
{
	/* Preload variables with adequate memory ordering */
	const size_t read = m_readCount.load(std::memory_order_acquire);
	size_t write = m_writeCount.load(std::memory_order_relaxed);
	const size_t free = CalcFree(write, read);
	const size_t cnt = std::min(size, free);
	if (cnt == 0) return 0;

	if (write + cnt <= size_)
	{
		memcpy(&m_data[write * elementSize_], data, cnt * elementSize_);
		write = (write + cnt) % size_;
	}
	else
	{
		const size_t linear_free = size_ - write;
		memcpy(&m_data[write * elementSize_], data, linear_free * elementSize_);
		const size_t remaining = cnt - linear_free;
		memcpy(&m_data[0], static_cast<const std::byte *>(data) + linear_free * elementSize_, remaining * elementSize_);
		write = remaining;
	}

	/* Store the write index with adequate ordering */
	m_writeCount.store(write, std::memory_order_release);

	return cnt;
}

size_t RingBuffer::Read(void *data, const size_t size)
{
	/* Preload variables with adequate memory ordering */
	const size_t write = m_writeCount.load(std::memory_order_acquire);
	size_t read = m_readCount.load(std::memory_order_relaxed);
	const size_t available = CalcAvailable(write, read);
	const size_t cnt = std::min(size, available);
	if (cnt == 0) return 0;

	if (read + cnt <= size_)
	{
		memcpy(data, &m_data[read * elementSize_], cnt * elementSize_);
		read = (read + cnt) % size_;
	}
	else
	{
		const size_t linear_available = size_ - read;
		memcpy(data, &m_data[read * elementSize_], linear_available * elementSize_);
		const size_t remaining = cnt - linear_available;
		memcpy(static_cast<std::byte *>(data) + linear_available * elementSize_,
		       &m_data[0],
		       remaining * elementSize_);
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
	return size_ - (w - r) - 1U;
}

size_t RingBuffer::CalcAvailable(const size_t w, const size_t r) const
{
	if (w >= r)
	{
		return w - r;
	}
	return size_ - (r - w);
}
