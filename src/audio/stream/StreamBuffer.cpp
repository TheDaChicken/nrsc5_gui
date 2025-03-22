//
// Created by TheDaChicken on 1/9/2025.
//

#include "StreamBuffer.h"

#include <pa_util.h>
#include <utils/Log.h>

StreamBuffer::StreamBuffer()
	: size_(0), element_size_(0), sample_rate_(0), driver_latency_(0), latency_size_(0),
	  read_time_(0),
	  write_count_(0),
	  read_count_(0),
	  read_count_next_(0),
	  frames_missing_(0),
	  latency_pending_(0)
{
}

int StreamBuffer::Initialize(
	const int size, const int latency_size, const double driver_latency, const int element_size, const int sample_rate
)
{
	element_size_ = element_size;
	sample_rate_ = sample_rate;

	size_ = size;
	latency_size_ = latency_size;
	driver_latency_ = driver_latency;

	Logger::Log(debug,
	            "StreamBuffer: Initializing buffer latency={} driver={} element_size={} sample_rate={}",
	            latency_size_.load(),
	            driver_latency_,
	            element_size_,
	            sample_rate_);

	ring_buffer.Open(GenerateBufferSize(), element_size);
	return 0;
}

int StreamBuffer::Write(const void *buffer, unsigned int sample_size)
{
	const std::size_t frames_missing = frames_missing_.exchange(0);
	if (frames_missing > 0)
	{
		// Log underflow
		Logger::Log(warn, "StreamBuffer: Output underflow {}", frames_missing);
	}

	const std::size_t frames_available = ring_buffer.GetFree();
	if (frames_available < sample_size)
	{
		// Log overflow
		Logger::Log(err, "StreamBuffer: Output overflow {}", sample_size - frames_available);
		return -1;
	}

	// Write the buffer
	const std::size_t frames_written = ring_buffer.Write(buffer, sample_size);

	// Increase our buffer delay if we haven't delayed enough
	if (latency_size_ > latency_pending_)
	{
		latency_pending_ = std::min(latency_pending_ + frames_written, latency_size_.load());
	}

	// Update our write count for our writing clock
	write_count_ += frames_written;
	return 0;
}

int StreamBuffer::Read(void *buffer, unsigned int sample_size, double output_time)
{
	std::size_t frames_read;
	std::size_t frames_missing;

	if (IsLatencyOk())
	{
		// If we got enough frames, start reading
		frames_read = ring_buffer.Read(buffer, sample_size);
		frames_missing = sample_size - frames_read;
	}
	else
	{
		// We are waiting for enough frames to be written for good playback
		frames_read = 0;
		frames_missing = sample_size;
	}

	read_count_ = read_count_next_.load();
	read_time_  = output_time;

	if (frames_missing > 0)
	{
		// Fill silence if underflow
		memset(static_cast<std::byte *>(buffer) + frames_read * element_size_,
		       0,
		       frames_missing * element_size_);

		// Report missing frames
		if (IsLatencyOk())
		{
			frames_missing_ += frames_missing;

			// Force the buffer to catch up with the missing frames
			latency_pending_ = std::min(latency_pending_ - frames_missing, static_cast<std::size_t>(0));
		}
	}

	read_count_next_ += frames_read;
	return 0;
}

double StreamBuffer::GetWrittenTime() const
{
	unsigned int latency_delay = latency_size_ - latency_pending_;
	unsigned int write_delay = write_count_ - read_count_;

	return read_time_.load() + static_cast<double>(latency_delay + write_delay) / sample_rate_;
}

void StreamBuffer::Reset()
{
	// Reset the write count
	write_count_ = 0;

	// Reset the read count
	read_count_ = 0;
	read_count_next_ = 0;

	// Reset the latency to fill the buffer again
	latency_pending_ = 0;

	ring_buffer.Flush();
}

std::size_t StreamBuffer::GenerateBufferSize() const
{
	// Calculate the buffer size based on the latency and driver latency
	return static_cast<std::size_t>(driver_latency_ * sample_rate_ + 1) + latency_size_ + size_;
}
