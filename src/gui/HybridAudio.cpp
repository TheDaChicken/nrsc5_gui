//
// Created by TheDaChicken on 11/9/2025.
//

#include "HybridAudio.h"

#include "utils/Log.h"

extern "C" {
#include <nrsc5.h>
}

#include <SDL3/SDL_audio.h>

bool HybridAudio::OpenAudio()
{
	SDL_AudioSpec spec;

	spec.channels = 2;
	spec.freq = NRSC5_SAMPLE_RATE_AUDIO;
	spec.format = SDL_AUDIO_S16LE;

	// TODO: Close audio. I need to make sure to make a good design for that.
	//output_audio = AUDIO::SDLAudioManager::GetInstance().CreateStream(&spec);
	// if (!output_audio)
	// {
	// 	Logger::Log(err, "Failed to create audio output stream");
	// 	return false;
	// }

	//SDL_SetAudioStreamGetCallback(output_audio->stream.get(), Callback, this);
	return true;
}

void HybridAudio::ReceiveEvent(const NRSC5::AudioFrame &frame)
{
	auto event = std::make_unique<AudioFrame>();

	const unsigned int size = frame.data.size() * sizeof(int16_t);

	event->program_id = frame.program_id;
	event->data.resize(size);
	memcpy(event->data.data(), frame.data.data(), size);

	std::lock_guard lock(mutex_);
	audio_queue_.emplace(std::move(event));
}

void HybridAudio::ChangeProgram(const unsigned int program)
{
	auto frame = std::make_unique<ProgramChangeEvent>();
	frame->program_id = program;

	std::lock_guard lock(mutex_);
	queue_.emplace(std::move(frame));
}

void HybridAudio::Callback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
{
	const auto controller = static_cast<HybridAudio *>(userdata);
	controller->OnAudioCallback(additional_amount);
}

void HybridAudio::OnAudioCallback(const int samples_needed)
{
	std::size_t samples_got = 0;
	std::lock_guard lock(mutex_);

	HandleMessages();

	while (!audio_queue_.empty() && samples_got < samples_needed)
	{
		const auto frame =
				std::move(audio_queue_.front());
		audio_queue_.pop();

		if (program_id_ != frame->program_id)
			continue;

		if (!SDL_PutAudioStreamData(
				output_audio->stream.get(),
				frame->data.data(),
				static_cast<int>(frame->data.size())))
		{
			Logger::Log(err, "Failed to queue audio data: {}", SDL_GetError());
		}

		samples_got += frame->data.size();
	}
}

void HybridAudio::HandleMessages()
{
	while (!queue_.empty())
	{
		auto message = std::move(queue_.front());
		queue_.pop();

		if (const auto program_change = dynamic_cast<ProgramChangeEvent*>(
			message.get()))
		{
			program_id_ = program_change->program_id;

			if (programChange)
				programChange(program_id_);
		}
	}
}
