//
// Created by TheDaChicken on 9/7/2025.
//

#ifndef HYBRIDSTREAM_H
#define HYBRIDSTREAM_H

#include "platform/sdl/SDLAudioManager.h"
#include "HybridAudio.h"
#include "controllers/Events.h"
#include "controllers/StationController.h"
#include "gui/managers/EventsDispatcher.h"
#include "input/RadioInput.h"

class UISession final
{
	public:
		explicit UISession();

		void Process();
		bool OpenAudio();

		std::shared_ptr<IRadioInput> GetSource()
		{
			return input_;
		}

		void SetInput(const std::shared_ptr<IRadioInput> &input)
		{
			// if (input_)
			// 	input_->Unsubscribe(audio_cont);
			//
			// input_ = input;
			// input_->Subscribe(audio_cont);
		}

		void SetChannel(const Station &station)
		{
			if (!input_)
				return;

			// if (input_->SetStation(station))
			// 	SetProgramId(station.program_id);
		}

		void SetProgramId(const unsigned int program) const
		{
			audio_cont->ChangeProgram(program);
		}

		void NextChannel();
		void PreviousChannel();

		// HybridState GetHybridState() const
		// {
		// 	return state_;
		// }

		// RadioStatus GetStatus()
		// {
		// 	if (!input_)
		// 		return Waiting;
		// 	return input_->GetStatus();
		// }

	private:
		void PushFrame(std::unique_ptr<EventData> &&event);
		void ProcessEvent(const std::unique_ptr<EventData> &event);
	private:
		std::shared_ptr<IRadioInput> input_;

		EventsTimerDispatcher events_dispatcher_;
		//HybridState state_;
		StationController station_cont;
		std::shared_ptr<HybridAudio> audio_cont = std::make_shared<HybridAudio>();
};

#endif //HYBRIDSTREAM_H
