//
// Created by TheDaChicken on 9/1/2025.
//

#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <atomic>
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_events.h>

#include "utils/Log.h"

namespace AUDIO
{
struct Device
{
	int id;
	std::string name;
};

using AUDIO_STREAM_PTR = std::unique_ptr<SDL_AudioStream, decltype(&SDL_DestroyAudioStream)>;
using AUDIO_DEVICE_CALLBACK = std::function<void(const AUDIO::Device&)>;

struct Track
{
	AUDIO_STREAM_PTR stream{nullptr, nullptr};
	SDL_AudioSpec input_spec{};
	float volume = 1.0;

	[[nodiscard]] std::chrono::nanoseconds Latency() const
	{
		return std::chrono::nanoseconds(
			static_cast<uint64_t>(SDL_GetAudioStreamQueued(stream.get())) * 1000000000 /
			SDL_AUDIO_FRAMESIZE(input_spec) /
			input_spec.freq);
	}
};

class SDLAudioManager
{
	public:
		using CLOCK = std::chrono::steady_clock;
		using TIME_POINT = CLOCK::time_point;

		SDLAudioManager();
		~SDLAudioManager();

		bool OpenAudioDevice(const Device *device);
		bool UpdateStreams();

		const char *GetCurrentDeviceName() const
		{
			return SDL_GetAudioDeviceName(mixer_id_);
		}

		std::shared_ptr<Track> CreateStream(const SDL_AudioSpec *spec);

		[[nodiscard]] TIME_POINT GetLastCallbackTime() const
		{
			return last_callback_;
		}

		std::map<SDL_AudioDeviceID, Device>& GetDevices()
		{
			return devices_;
		}

	private:
		void Callback(SDL_AudioStream *stream, int additional_amount, int total_amount);

		AUDIO_STREAM_PTR output_stream_;
		SDL_AudioSpec mixer_spec_;
		SDL_AudioDeviceID mixer_id_;
		SDL_EventFilter event_filter_;

		std::map<SDL_AudioDeviceID, Device> devices_;
		std::vector<std::shared_ptr<Track> > streams_;
		std::vector<float> mixer_buffer_;
		std::atomic<TIME_POINT> last_callback_;
};
} // namespace AUDIO

#endif //AUDIOMANAGER_H
