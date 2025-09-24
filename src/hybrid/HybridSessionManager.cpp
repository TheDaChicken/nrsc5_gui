//
// Created by TheDaChicken on 9/7/2025.
//

#include "HybridSessionManager.h"

HybridSessionManager::HybridSessionManager()
{
}

void HybridSessionManager::Subscribe(const std::shared_ptr<HybridSession> &session)
{
	subscriptions_.emplace_back(std::weak_ptr(session));

	// Send current state
	{
		auto freq_frame = std::make_unique<FrequencyChangeFrame>();
		freq_frame->mode = state_.mode;
		freq_frame->freq = state_.freq;
		session->SendEvent(std::move(freq_frame));
	}

	if (!state_.name.empty())
	{
		auto name_frame = std::make_unique<StationNameFrame>();
		name_frame->name = state_.name;
		session->SendEvent(std::move(name_frame));
	}

	if (state_.id != 0)
	{
		auto id_frame = std::make_unique<StationIdFrame>();
		id_frame->country_code = state_.country_code;
		id_frame->id = state_.id;
		session->SendEvent(std::move(id_frame));
	}
}

void HybridSessionManager::Unsubscribe(const std::shared_ptr<HybridSession> &session)
{
	subscriptions_.erase(
		std::remove_if(
			subscriptions_.begin(),
			subscriptions_.end(),
			[&session](const std::weak_ptr<HybridSession> &ptr)
			{
				const auto locked = ptr.lock();
				return locked && locked == session;
			}), subscriptions_.end());
}

void HybridSessionManager::PushCurrentProgramData(const std::shared_ptr<HybridSession> &session)
{
	const auto program_id = session->GetCurrentProgramId();

	auto new_program = std::make_unique<ProgramIdChangeFrame>();
	new_program->program_id = program_id;

	session->SendEvent(std::move(new_program));

	const auto program_itr = programs_cache.find(program_id);
	if (program_itr == programs_cache.end())
		return;

	const auto &prog = program_itr->second;

	{
		auto program_frame = std::make_unique<StationProgramFrame>();
		program_frame->id = program_id;
		program_frame->type = prog.type;
		session->SendEvent(std::move(program_frame));
	}

	{
		auto id3_frame = std::make_unique<StationId3Frame>();
		id3_frame->id3 = prog.id3;
		session->SendEvent(std::move(id3_frame));
	}
}

void HybridSessionManager::HandleEvents(const std::unique_ptr<EventData> &ptr)
{
	EventData *frame = ptr.get();

	switch (frame->type)
	{
		case FREQUENCY_CHANGE:
		{
			OnFrequencyChange(dynamic_cast<FrequencyChangeFrame *>(frame));
			break;
		}
		case PROGRAM_CHANGE:
		{
			OnProgramChange(dynamic_cast<ProgramIdChangeFrame *>(frame));
			break;
		}
		case STATION_NAME:
		{
			OnStationName(dynamic_cast<StationNameFrame *>(frame));
			break;
		}
		case STATION_ID:
		{
			OnStationId(dynamic_cast<StationIdFrame *>(frame));
			break;
		}
		case STATION_PROGRAM:
		{
			OnProgramDiscovered(dynamic_cast<StationProgramFrame *>(frame));
			break;
		}
		case STATION_ID3:
		{
			OnId3Update(dynamic_cast<StationId3Frame *>(frame));
			break;
		}
		default:
			break;
	}
}

void HybridSessionManager::HandleAudio(const std::unique_ptr<StationAudioFrame> &audio_frame) const
{
	for (auto &weak_session : subscriptions_)
	{
		const auto session = weak_session.lock();
		if (!session)
			continue;

		if (session->GetCurrentProgramId() != audio_frame->program_id)
			continue;

		session->SendAudio(audio_frame);
	}
}

void HybridSessionManager::OnFrequencyChange(const FrequencyChangeFrame *frame)
{
	state_.Reset();
	programs_cache.clear();

	state_.mode = frame->mode;
	state_.freq = frame->freq;

	state_.frequency_text = Band::FormatFrequency(
		state_.mode,
		state_.freq
	);

	// Reset to analog
	for (const auto &session_weak : subscriptions_)
	{
		if (const auto session = session_weak.lock())
		{
			session->SendStation({});
			session->SendProgram(programs_cache[0]);
		}
	}
}

void HybridSessionManager::OnProgramChange(const ProgramIdChangeFrame *frame) const
{
	ProgramState dummy;
	dummy.id = frame->program_id;

	const auto iter = programs_cache.find(frame->program_id);
	if (iter != programs_cache.end())
		dummy = iter->second;

	for (const auto &session_weak : subscriptions_)
	{
		if (const auto session = session_weak.lock())
		{
			session->SendProgram(dummy);
		}
	}
}

void HybridSessionManager::OnStationName(const StationNameFrame *frame)
{
	state_.name = frame->name;

	if (!state_.IsDry())
		StationUpdate();
}

void HybridSessionManager::OnStationId(const StationIdFrame *frame)
{
	state_.country_code = frame->country_code;
	state_.id = frame->id;

	if (!state_.IsDry())
		StationUpdate();
}

void HybridSessionManager::OnProgramDiscovered(const StationProgramFrame *frame)
{
	auto &prog = programs_cache[frame->id];

	prog.id = frame->id;
	prog.type = frame->type;

	for (const auto &session_weak : subscriptions_)
	{
		if (const auto session = session_weak.lock())
		{
			if (session->GetCurrentProgramId() != frame->id)
				continue;

			auto program_frame = std::make_unique<StationProgramFrame>();
			program_frame->id = frame->id;
			program_frame->type = frame->type;

			session->SendEvent(std::move(program_frame));
		}
	}
}

void HybridSessionManager::OnId3Update(const StationId3Frame *frame)
{
	auto &prog = programs_cache[frame->id3.program_id];

	prog.id3 = frame->id3;

	for (const auto &session_weak : subscriptions_)
	{
		if (const auto session = session_weak.lock())
		{
			if (session->GetCurrentProgramId() != prog.id3.program_id)
				continue;

			auto id3_frame = std::make_unique<StationId3Frame>();
			id3_frame->id3 = prog.id3;

			session->SendEvent(std::move(id3_frame));
		}
	}
}

void HybridSessionManager::StationUpdate() const
{
	const StationIdentity identity = GetIdentity();

	for (const auto &session_weak : subscriptions_)
	{
		if (const auto session = session_weak.lock())
		{
			session->SendStation(identity);
		}
	}
}
