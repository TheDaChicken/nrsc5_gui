//
// Created by TheDaChicken on 8/3/2025.
//

#include "HybridDecoder.h"

#include "utils/Log.h"

HybridDecoder::HybridDecoder()
{

}

HybridDecoder::~HybridDecoder()
= default;

tl::expected<void, NRSC5::StreamStatus> HybridDecoder::Open(
	const NRSC5::StreamSupported &input)
{
	tuner_mode_ = input.tuner_mode;

	if (tuner_mode_ == TunerMode::ArbResampler)
		return stream_processor_.Open(input);
	return {};
}

void HybridDecoder::Reset(const float freq)
{
	std::lock_guard lock(mutex_);

	if (int ret = decoder_.Reset(freq) < 0)
	{
		Logger::Log(err, "Failed to reset NRSC5 Decoder {}", ret);
		//return tl::unexpected(StreamStatus_Error);
	}

	// TODO: add ret
	stream_processor_.Reset();
}

void HybridDecoder::SetMode(Band::Type mode)
{
	std::lock_guard lock(mutex_);

	// TODO: The sample rate changes based on mode
	decoder_.SetMode(mode);
	stream_processor_.SetMode(mode);
}

void HybridDecoder::ProcessSamples(
	const RadioFrame &transfer)
{
	std::lock_guard lock(mutex_);

	switch (tuner_mode_)
	{
		case TunerMode::Native:
		{
			decoder_.PipeSamplesCU8(
				reinterpret_cast<const uint8_t *>(transfer.data.data()),
				transfer.frame_size * 2);
			break;
		}
		case TunerMode::ArbResampler:
		{
			auto [data, size] = stream_processor_.Process(
				transfer.data.data(),
				transfer.frame_size);

			decoder_.PipeSamplesCS16(static_cast<const int16_t *>(data),
			                         size * 2);
			break;
		}
		default:
		{
			Logger::Log(err,
			            "Unsupported tuner mode: {}",
			            static_cast<int>(tuner_mode_));
			break;
		}
	}
}
