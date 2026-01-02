//
// Created by TheDaChicken on 7/7/2025.
//

#ifndef HISTORYBUFFER_H
#define HISTORYBUFFER_H

#include <volk/volk_alloc.hh>

template<class T>
class HistoryBuffer
{
	public:
		HistoryBuffer();

		void SetHistorySize(const size_t size)
		{
			m_history = size;
		}

		void Reset()
		{
			if (m_history == 0)
				return;

			read_idx = 0;
			write_idx = m_history - 1;
		}

		int GetBufferSize() const
		{
			return m_buffer_size;
		}

		unsigned int WriteAvailable() const
		{
			return m_buffer_size - write_idx - 1;
		}

		unsigned int ReadAvailable() const
		{
			return write_idx - read_idx;
		}

		void SetBlockSize(size_t size);
		void Write(const T *data, size_t elementSize);
		void Consume(size_t n);

		T *Get()
		{
			if (m_buffer == nullptr)
				throw std::runtime_error("Buffer not initialized");

			return &m_buffer[read_idx];
		}

	private:
		std::size_t m_align;
		int m_naligned;
		volk::vector<T> m_buffer_ptr;
		T *m_buffer;
		int write_idx = 0;
		int read_idx = 0;
		std::size_t m_buffer_size = 0;
		std::size_t m_history = 0;
};

template<class T>
HistoryBuffer<T>::HistoryBuffer()
	: m_align(volk_get_alignment()),
	  m_naligned(std::max(static_cast<size_t>(1), m_align / sizeof(T))),
	  m_buffer(nullptr)
{
}

template<class T>
void HistoryBuffer<T>::SetBlockSize(const size_t size)
{
	if (m_history == 0)
		throw std::runtime_error("History size is not set");

	m_buffer_size = size * m_history;
	m_buffer_ptr.clear();
	m_buffer_ptr.resize(size * (m_history + m_naligned));
	m_buffer = &m_buffer_ptr[m_naligned];

	memset(m_buffer, 0, m_buffer_size * sizeof(T));
}

template<class T>
void HistoryBuffer<T>::Write(const T *data, const size_t elementSize)
{
	if (m_buffer == nullptr || elementSize == 0)
		return;

	if (write_idx + elementSize > m_buffer_size)
		throw std::runtime_error("HistoryBuffer overflow");

	std::copy(data, data + elementSize, m_buffer + write_idx);
	write_idx += elementSize;
}

template<class T>
void HistoryBuffer<T>::Consume(const size_t n)
{
	if (m_buffer == nullptr || n == 0)
		return;

	if (m_buffer_size > 0 && ReadAvailable() < n)
		throw std::runtime_error("Invalid consume size");

	read_idx += n;

	if (read_idx >= m_history)
	{
		// Shift the buffer to the left to make space for new data
		std::move(m_buffer + read_idx, m_buffer + write_idx, m_buffer);

		write_idx -= read_idx;
		read_idx = 0;
	}
}

#endif //HISTORYBUFFER_H
