//
// Created by TheDaChicken on 9/1/2025.
//

#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <atomic>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <vector>
#include <SDL3/SDL_audio.h>

namespace AUDIO
{
struct Device
{
	int id;
	std::string_view name;
};

struct Track
{
	Track()
	: stream(nullptr, SDL_DestroyAudioStream)
	{

	}

	std::unique_ptr<SDL_AudioStream, decltype(&SDL_DestroyAudioStream)> stream;
	SDL_AudioSpec input_spec{};
	SDL_AudioSpec output_spec{};
	float volume = 1.0;

	[[nodiscard]] std::chrono::nanoseconds Latency() const
	{
		return std::chrono::nanoseconds(
			SDL_GetAudioStreamQueued(stream.get()) * 1000000000 /
			SDL_AUDIO_FRAMESIZE(input_spec) /
			input_spec.freq);
	}
};

class AudioManager
{
	public:
		AudioManager()
			: sdl_stream_(nullptr, SDL_DestroyAudioStream)
		{

		}

		static AudioManager &GetInstance()
		{
			static AudioManager instance;
			return instance;
		}

		static std::vector<Device> GetPlaybackDevices()
		{
			std::vector<Device> devices;
			int len = 0;

			SDL_AudioDeviceID *array = SDL_GetAudioPlaybackDevices(&len);

			devices.resize(len);
			for (int i = 0; i < len; ++i)
			{
				auto &device = devices[i];
				device.id = array[i];
				device.name = SDL_GetAudioDeviceName(array[i]);
			}
			SDL_free(array);
			return devices;
		}

		bool Initialize();
		bool Uninitialize()
		{
			streams_.clear();
			sdl_stream_.reset();
			return true;
		}
		bool OpenAudioDevice(const Device *device);
		bool UpdateStreams();

		[[nodiscard]] const SDL_AudioSpec &GetSpec() const
		{
			return spec_;
		}

		[[nodiscard]] int GetCurrentLogicalId() const
		{
			return logical_id_;
		}

		const char* GetCurrentDeviceName() const
		{
			return SDL_GetAudioDeviceName(logical_id_);
		}

		std::chrono::steady_clock::time_point GetLastCallbackTime() const
		{
			return last_callback_;
		}

		std::shared_ptr<Track> CreateStream(const SDL_AudioSpec* spec);
	private:
		void Callback(SDL_AudioStream *stream, int additional_amount, int total_amount);

		std::vector<std::shared_ptr<Track>> streams_;
		std::vector<float> mixer_buffer_;
		std::atomic<std::chrono::steady_clock::time_point> last_callback_;

		std::unique_ptr<SDL_AudioStream, decltype(&SDL_DestroyAudioStream)> sdl_stream_;
		int logical_id_ = 0;
		SDL_AudioSpec spec_{};
};
} // namespace AUDIO

#endif //AUDIOMANAGER_H
