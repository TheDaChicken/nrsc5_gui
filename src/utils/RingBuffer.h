//
// Created by TheDaChicken on 8/7/2025.
//

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <cstddef>
#include <atomic>
#include <vector>

class RingBuffer
{
	public:
		RingBuffer();
		~RingBuffer();

		void Open(size_t size, size_t element_size);
		void Close();

		void Flush();

		size_t Write(const void *data, size_t size);
		size_t Read(void *data, size_t size);

		[[nodiscard]] size_t GetFree() const;
		[[nodiscard]] size_t GetAvailable() const;

	private:
		[[nodiscard]] size_t CalcFree(size_t w, size_t r) const;
		[[nodiscard]] size_t CalcAvailable(size_t w, size_t r) const;

		std::vector<std::byte> m_data;
		size_t elementSize_;
		size_t size_;

		/* Read index */
		alignas(64) std::atomic_size_t m_readCount;
		/* Write index */
		alignas(64) std::atomic_size_t m_writeCount;
};

#endif //RINGBUFFER_H
