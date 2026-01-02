//
// Created by TheDaChicken on 11/9/2025.
//

#ifndef NRSC5_GUI_AUDIOCONTROLLER_H
#define NRSC5_GUI_AUDIOCONTROLLER_H

#include "platform/sdl/SDLAudioManager.h"
#include "gui/managers/EventsDispatcher.h"
#include "nrsc5/Station.h"

struct AudioFrame
{
	std::vector<char> data;
	unsigned int program_id;
};

struct ProgramChangeEvent final : EventData
{
	unsigned int program_id{0};
};

class HybridAudio final
		// : public IHybridSession,
		//   public std::enable_shared_from_this<IHybridSession>
{
	public:
		bool OpenAudio();
		void ReceiveEvent(const NRSC5::AudioFrame &frame) /*override */;

		void ChangeProgram(unsigned int program);

		bool NeedMoreAudio() /*override */
		{
			return audio_queue_.size() < 25;
		}

		[[nodiscard]] std::chrono::nanoseconds Latency() const
		{
			if (!output_audio)
				return std::chrono::nanoseconds(0);
			return output_audio->Latency();
		}

		std::chrono::steady_clock::time_point GetCurrentTick()
		{
			return {};
			//return AUDIO::SDLAudioManager::GetInstance().GetLastCallbackTime();
		}

		std::function<void(unsigned int program)> programChange;

	private:
		static void Callback(
			void *userdata, SDL_AudioStream *stream,
			int additional_amount, int total_amount);

		std::queue<std::unique_ptr<EventData> > queue_;
		std::queue<std::unique_ptr<AudioFrame> > audio_queue_;

		void OnAudioCallback(int samples_needed);
		void HandleMessages();

		unsigned int program_id_{0};
		std::mutex mutex_;
		std::shared_ptr<AUDIO::Track> output_audio;
};

#endif //NRSC5_GUI_AUDIOCONTROLLER_H
