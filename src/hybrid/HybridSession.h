//
// Created by TheDaChicken on 9/9/2025.
//

#ifndef HYBRIDSESSION_H
#define HYBRIDSESSION_H

#include <memory>

#include "HybridController.h"
#include "HybridState.h"
#include "audio/AudioManager.h"
#include "external/HybridExternal.h"
#include "gui/managers/EventsDispatcher.h"

struct ProgramChangeFrame final : EventData
{
	explicit ProgramChangeFrame()
		: EventData(PROGRAM_CHANGE)
	{
	}

	ProgramState state;
};

class HybridSession
{
	public:
		explicit HybridSession(const std::shared_ptr<HybridExternal> &external);

		bool OpenAudio();

		void SendEvent(std::unique_ptr<EventData> &&event);
		void SendAudio(const std::unique_ptr<StationAudioFrame> &audio_frame);

		void Process();

		void SendProgram(const ProgramState& state)
		{
			auto new_frame = std::make_unique<ProgramChangeFrame>();
			new_frame->state = state;
			frame_dispatcher.PushFrame(std::move(new_frame));
		}

		void SendStation(const StationIdentity &identity);
		void FetchStationImage();
		void FetchPrimaryImage(int xhdr);

		int GetCurrentProgramId() const
		{
			return program_.id;
		}

		ProgramState &GetState()
		{
			return program_;
		}
		void OnProgramUpdate(const StationProgramFrame *ptr);

	private:
		void OnProgramStateChange(const ProgramChangeFrame *frame);
		void OnId3Update(const StationId3Frame *frame);

		ProgramState program_;
		StationIdentity identity_;

		EventsDispatcher frame_dispatcher;
		std::shared_ptr<AUDIO::Track> output_audio;

		std::shared_ptr<HybridExternal> external_service_;
};

#endif //HYBRIDSESSION_H
