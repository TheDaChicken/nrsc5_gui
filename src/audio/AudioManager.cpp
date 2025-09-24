//
// Created by TheDaChicken on 9/1/2025.
//

#include "AudioManager.h"

#include <algorithm>
#include <cstring>
#include <SDL3/SDL_events.h>

#include "utils/Log.h"

bool AUDIO::AudioManager::Initialize()
{
	SDL_AddEventWatch(
		[](void *userdata, SDL_Event *event)
		{
			auto *manager = static_cast<AudioManager *>(userdata);
			if (event->type != SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED)
			{
				return false;
			}
			if (event->adevice.which != manager->logical_id_)
			{
				return false;
			}
			if (!manager->sdl_stream_)
			{
				return false;
			}

			Logger::Log(info, "Audio device format changed, changing output format");
			if (SDL_GetAudioStreamFormat(manager->sdl_stream_.get(), nullptr, &manager->spec_))
			{
				manager->spec_.format = SDL_AUDIO_F32;
				manager->UpdateStreams();
			}

			return true;
		},
		this);
	return OpenAudioDevice(nullptr);
}

bool AUDIO::AudioManager::OpenAudioDevice(const Device *device)
{
	SDL_AudioDeviceID inputId = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK;
	if (device)
		inputId = device->id;

	spec_.channels = 2;
	spec_.freq = 48000;
	spec_.format = SDL_AUDIO_F32;

	auto new_stream = std::unique_ptr<SDL_AudioStream, decltype(&SDL_DestroyAudioStream)>(
		SDL_OpenAudioDeviceStream(
			inputId,
			&spec_,
			[](void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
			{
				auto *manager = static_cast<AudioManager *>(
					userdata);
				manager->Callback(
					stream,
					additional_amount,
					total_amount);
			},
			this),
		SDL_DestroyAudioStream);
	if (!new_stream)
	{
		Logger::Log(err, "Failed to open audio device stream: {}", SDL_GetError());
		return false;
	}

	SDL_AudioSpec spec;
	if (!SDL_GetAudioStreamFormat(new_stream.get(), nullptr, &spec))
	{
		Logger::Log(err, "Failed to get audio device format: {}", SDL_GetError());
		return false;
	}
	const int logical_id = SDL_GetAudioStreamDevice(new_stream.get());
	if (logical_id == 0)
	{
		Logger::Log(err, "Failed to get audio device ID: {}", SDL_GetError());
		return false;
	}

	spec_ = spec;
	logical_id_ = logical_id;
	sdl_stream_ = std::move(new_stream);
	SDL_ResumeAudioStreamDevice(sdl_stream_.get());

	UpdateStreams();
	return true;
}

bool AUDIO::AudioManager::UpdateStreams()
{
	for (const auto &track : streams_)
	{
		if (!SDL_SetAudioStreamFormat(track->stream.get(), nullptr, &spec_))
		{
			Logger::Log(err, "Failed to set audio stream format: {}", SDL_GetError());
			continue;
		}

		if (!SDL_GetAudioStreamFormat(track->stream.get(), nullptr, &track->output_spec))
		{
			Logger::Log(err, "Failed to get audio stream format: {}", SDL_GetError());
		}
	}
	return true;
}

std::shared_ptr<AUDIO::Track> AUDIO::AudioManager::CreateStream(const SDL_AudioSpec *spec)
{
	if (!sdl_stream_)
	{
		Logger::Log(err, "Audio device not opened");
		return {};
	}

	auto track = std::make_shared<Track>();

	track->stream = std::unique_ptr<SDL_AudioStream, decltype(&SDL_DestroyAudioStream)>(
		SDL_CreateAudioStream(spec, &spec_),
		SDL_DestroyAudioStream);
	if (!track->stream)
	{
		Logger::Log(err, "Failed to create audio stream: {}", SDL_GetError());
		return {};
	}
	if (!SDL_GetAudioStreamFormat(track->stream.get(), nullptr, &track->output_spec))
	{
		Logger::Log(err, "Failed to get audio stream format: {}", SDL_GetError());
		return {};
	}

	track->input_spec = *spec;

	streams_.push_back(std::move(track));
	return streams_.back();
}

static void MixFloat32Audio(float *dst, const float *src, const int buffer_size, const float gain)
{
	if (gain == 0.0f)
		return; // don't mix silence.

	if (!SDL_MixAudio(reinterpret_cast<Uint8 *>(dst),
	                  reinterpret_cast<const Uint8 *>(src),
	                  SDL_AUDIO_F32,
	                  buffer_size,
	                  gain))
	{
		assert(!"This shouldn't happen.");
	}
}

void AUDIO::AudioManager::Callback(SDL_AudioStream *stream, const int additional_amount, int total_amount)
{
	if (additional_amount == 0)
		return; // don't need to generate more audio yet.

	assert((additional_amount % sizeof (float)) == 0);

	const int size = additional_amount / sizeof(float) * 2;
	if (size > mixer_buffer_.size())
	{
		mixer_buffer_.resize(size);
	}

	float *buffer = mixer_buffer_.data();
	float *final_buff = buffer + (additional_amount / sizeof(float));

	memset(final_buff, 0, additional_amount);

	for (const auto &track : streams_)
	{
		const int to_be_read = additional_amount / SDL_AUDIO_FRAMESIZE(spec_) * SDL_AUDIO_FRAMESIZE(
			track->output_spec);

		const int read = SDL_GetAudioStreamData(track->stream.get(), buffer, to_be_read);
		assert(track->output_spec.channels == spec_.channels);

		if (read > 0)
			MixFloat32Audio(final_buff, buffer, read, 1.0);
		else if (read < 0)
			Logger::Log(err, "Failed to get audio stream data: {}", SDL_GetError());
	}

	last_callback_ = std::chrono::steady_clock::now();

	SDL_PutAudioStreamData(stream, final_buff, additional_amount);
}
