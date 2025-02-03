//
// Created by TheDaChicken on 1/9/2025.
//

#ifndef STREAMBUFFER_H
#define STREAMBUFFER_H

#include <cstddef>
#include <utils/RingBuffer.h>

class StreamBuffer
{
  public:
    StreamBuffer();

    int Initialize(int size, int latency_size, double driver_latency, int element_size, int sample_rate);

    int Write(const void *buffer, unsigned int sample_size);
    int Read(void *buffer, unsigned int sample_size, double output_time);
    void Reset();

    /**
     * Get the time of the last write operation.
     */
    double GetWrittenTime() const;

    bool IsLatencyOk() const
    {
      return latency_size_ <= latency_pending_;
    }

  private:
    std::size_t GenerateBufferSize() const;

    RingBuffer ring_buffer;

    int element_size_;
    int sample_rate_;

    int size_;
    double driver_latency_;
    std::atomic<std::size_t> latency_size_;

    std::atomic<double> read_time_;

    std::atomic<std::size_t> write_count_;
    std::atomic<std::size_t> read_count_;
    std::atomic<std::size_t> read_count_next_;

    std::atomic<std::size_t> frames_missing_;
    std::atomic<std::size_t> latency_pending_;
};

#endif //STREAMBUFFER_H
