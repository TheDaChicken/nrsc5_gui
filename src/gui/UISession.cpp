//
// Created by TheDaChicken on 9/7/2025.
//

#include "UISession.h"

UISession::UISession()
	//: station_cont(state_)
{
}

void UISession::Process()
{
	// if (input_)
	// 	state_.data = input_->Metadata().GetStateData();

	audio_cont->programChange = [this](const unsigned int program)
	{
		station_cont.ProgramChanged(program);
	};

	events_dispatcher_.Tick([this](const std::unique_ptr<EventData> &event)
	{
		ProcessEvent(event);
	});
}

bool UISession::OpenAudio()
{
	return audio_cont->OpenAudio();
}

void UISession::NextChannel()
{
	// const auto itr = state_.data.programs.upper_bound(state_.ui.program_id);
	// if (itr != state_.data.programs.end())
	// {
	// 	SetProgramId(itr->first);
	// 	// session->SendProgram(itr->second);
	// }
	// else
	// {
	// 	// TODO: Scan for next Station
	// }
}

void UISession::PreviousChannel()
{
	// auto itr = state_.data.programs.lower_bound(state_.ui.program_id);
	// if (itr != state_.data.programs.begin())
	// {
	// 	--itr;
	// 	SetProgramId(itr->first);
	// 	// session->SendProgram(itr->second);
	// }
	// else
	// {
	// 	// TODO: Scan for previous Station
	// }
}

void UISession::PushFrame(std::unique_ptr<EventData> &&event)
{
	// TODO: Event dispatcher needs a way to prevent repeated events.
	//  This can get filled up.

	// Set timestamp to current time + latency, so events are handled the same time as heard
	event->timestamp = audio_cont->GetCurrentTick() + audio_cont->Latency();

	events_dispatcher_.PushFrame(std::move(event));
}

void UISession::ProcessEvent(const std::unique_ptr<EventData> &event)
{
	// if (const auto hybridEvent = dynamic_cast<HybridEventFrame *>(event.get()))
	// {
	// 	station_cont.ProcessHybridEvent(hybridEvent->event);
	// }
	// if (const auto hybridLotFrame = dynamic_cast<HybridLotFrame *>(event.get()))
	// {
	// 	station_cont.ProcessLot(
	// 		hybridLotFrame->identity,
	// 		*std::static_pointer_cast<NRSC5::Lot>(hybridLotFrame->lot));
	// }
	// if (const auto hybridProgram = dynamic_cast<HybridProgram *>(event.get()))
	// 	station_cont.ProgramChanged(hybridProgram->program_id);
}

