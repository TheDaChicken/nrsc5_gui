//
// Created by TheDaChicken on 9/7/2025.
//

#ifndef HYBRIDSTREAM_H
#define HYBRIDSTREAM_H

#include "gui/managers/EventsDispatcher.h"
#include "external/HybridExternal.h"
#include "HybridSession.h"

class HybridSessionManager
{
	public:
		explicit HybridSessionManager();

		void Subscribe(const std::shared_ptr<HybridSession> &session);
		void Unsubscribe(const std::shared_ptr<HybridSession> &session);
		void PushCurrentProgramData(const std::shared_ptr<HybridSession> &session);

		void HandleEvents(const std::unique_ptr<EventData> &ptr);
		void HandleAudio(const std::unique_ptr<StationAudioFrame> &audio_frame) const;


		[[nodiscard]] const HybridState &GetState() const
		{
			return state_;
		}

		[[nodiscard]] StationIdentity GetIdentity() const
		{
			return StationIdentity{
				state_.id,
				state_.country_code,
				state_.name
			};
		}

		[[nodiscard]] const std::map<unsigned int, ProgramState> &GetPrograms() const
		{
			return programs_cache;
		}

		std::function<void(const Station &station)> on_tune;

	private:
		void OnFrequencyChange(const FrequencyChangeFrame *frame);
		void OnProgramChange(const ProgramIdChangeFrame *frame) const;
		void OnStationName(const StationNameFrame *frame);
		void OnStationId(const StationIdFrame *frame);
		void OnProgramDiscovered(const StationProgramFrame *frame);
		void OnId3Update(const StationId3Frame *frame);
		void StationUpdate() const;

		std::shared_ptr<HybridExternal> external_service_;

		HybridState state_;

		// program ID -> program
		std::map<unsigned int, ProgramState> programs_cache;

		std::vector<std::weak_ptr<HybridSession> > subscriptions_;
};

#endif //HYBRIDSTREAM_H
