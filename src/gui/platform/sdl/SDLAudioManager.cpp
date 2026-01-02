//
// Created by TheDaChicken on 9/1/2025.
//

#include "SDLAudioManager.h"

#include <algorithm>
#include <cstring>
#include <SDL3/SDL_events.h>

#include "utils/Log.h"

AUDIO::SDLAudioManager::SDLAudioManager()
	: output_stream_(nullptr, nullptr),
	  mixer_spec_(), mixer_id_(0)
{
	event_filter_ = [](void *userdata, SDL_Event *event)
	{
		auto *manager = static_cast<SDLAudioManager *>(userdata);
		if (event->type == SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED)
		{
			if (!manager->output_stream_ && event->adevice.which != manager->mixer_id_)
			{
				return false;
			}

			Logger::Log(info, "Audio device format changed, changing output format");

			if (SDL_GetAudioStreamFormat(
				manager->output_stream_.get(),
				nullptr,
				&manager->mixer_spec_))
			{
				manager->mixer_spec_.format = SDL_AUDIO_F32;
				manager->UpdateStreams();
			}

			return true;
		}
		if (event->type == SDL_EVENT_AUDIO_DEVICE_ADDED)
		{
			Device dev;

			dev.id = event->adevice.which;
			dev.name = SDL_GetAudioDeviceName(dev.id);

			manager->devices_.emplace(dev.id, dev);
		}
		if (event->type == SDL_EVENT_AUDIO_DEVICE_REMOVED)
		{
			manager->devices_.erase(event->adevice.which);
		}
		return false;
	};

	SDL_AddEventWatch(event_filter_, this);
}

AUDIO::SDLAudioManager::~SDLAudioManager()
{
	SDL_RemoveEventWatch(event_filter_, this);
}

bool AUDIO::SDLAudioManager::OpenAudioDevice(const Device *device)
{
	SDL_AudioDeviceID inputId = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK;
	if (device)
		inputId = device->id;

	SDL_AudioSpec spec;
	spec.channels = 2;
	spec.freq = 48000;
	spec.format = SDL_AUDIO_F32;

	auto new_stream = AUDIO_STREAM_PTR(
		SDL_OpenAudioDeviceStream(
			inputId,
			&spec,
			[](void *userdata,
			   SDL_AudioStream *stream,
			   int additional_amount,
			   int total_amount)
			{
				auto *manager = static_cast<SDLAudioManager *>(
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

	SDL_AudioSpec out_;
	if (!SDL_GetAudioStreamFormat(new_stream.get(), nullptr, &out_))
	{
		Logger::Log(err, "Failed to get audio device format: {}", SDL_GetError());
		return false;
	}
	const SDL_AudioDeviceID logical_id = SDL_GetAudioStreamDevice(new_stream.get());
	if (logical_id == 0)
	{
		Logger::Log(err, "Failed to get audio device ID: {}", SDL_GetError());
		return false;
	}

	output_stream_ = std::move(new_stream);
	mixer_spec_ = out_;
	mixer_id_ = logical_id;

	SDL_ResumeAudioStreamDevice(output_stream_.get());

	UpdateStreams();
	return true;
}

bool AUDIO::SDLAudioManager::UpdateStreams()
{
	for (const auto &track : streams_)
	{
		if (!SDL_SetAudioStreamFormat(
			track->stream.get(),
			&mixer_spec_,
			&track->input_spec))
		{
			Logger::Log(err, "Failed to set audio stream format: {}", SDL_GetError());
		}
	}
	return true;
}

std::shared_ptr<AUDIO::Track> AUDIO::SDLAudioManager::CreateStream(const SDL_AudioSpec *spec)
{
	if (!output_stream_)
	{
		Logger::Log(err, "Audio device not opened");
		return {};
	}

	auto track = std::make_shared<Track>();

	track->stream = std::unique_ptr<SDL_AudioStream, decltype(&SDL_DestroyAudioStream)>(
		SDL_CreateAudioStream(spec, &mixer_spec_),
		SDL_DestroyAudioStream);
	if (!track->stream)
	{
		Logger::Log(err, "Failed to create audio stream: {}", SDL_GetError());
		return {};
	}
	if (!SDL_GetAudioStreamFormat(
		track->stream.get(),
		nullptr,
		&track->input_spec))
	{
		Logger::Log(err, "Failed to get audio stream format: {}", SDL_GetError());
		return {};
	}

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

void AUDIO::SDLAudioManager::Callback(SDL_AudioStream *stream, const int additional_amount, int total_amount)
{
	if (additional_amount == 0)
		return; // don't need to generate more audio yet.

	assert((additional_amount % sizeof (float)) == 0);

	const std::size_t size = additional_amount / sizeof(float) * 2;
	if (size > mixer_buffer_.size())
	{
		mixer_buffer_.resize(size);
	}

	float *buffer = mixer_buffer_.data();
	float *final_buff = buffer + (additional_amount / sizeof(float));

	memset(final_buff, 0, additional_amount);

	for (const auto &track : streams_)
	{
		const int to_be_read = additional_amount /
				SDL_AUDIO_FRAMESIZE(mixer_spec_) * SDL_AUDIO_FRAMESIZE(
					track->input_spec);

		const int read = SDL_GetAudioStreamData(track->stream.get(), buffer, to_be_read);
		assert(track->input_spec.channels == mixer_spec_.channels);

		if (read > 0)
			MixFloat32Audio(final_buff, buffer, read, 1.0);
		else if (read < 0)
			Logger::Log(err, "Failed to get audio stream data: {}", SDL_GetError());
	}

	last_callback_ = CLOCK::now();

	SDL_PutAudioStreamData(stream, final_buff, additional_amount);
}
