//
// Created by TheDaChicken on 9/9/2025.
//

#include "HybridSession.h"

HybridSession::HybridSession(
	const std::shared_ptr<HybridExternal> &external)
	: external_service_(external)
{
}

bool HybridSession::OpenAudio()
{
	SDL_AudioSpec spec;

	spec.channels = 2;
	spec.freq = NRSC5_SAMPLE_RATE_AUDIO;
	spec.format = SDL_AUDIO_S16LE;

	output_audio = AUDIO::AudioManager::GetInstance().CreateStream(&spec);
	if (!output_audio)
	{
		Logger::Log(err, "Failed to create audio output stream");
		return false;
	}

	return true;
}

void HybridSession::SendEvent(std::unique_ptr<EventData> &&event)
{
	// Set timestamp to current time + latency, so events are handled the same time heard
	std::chrono::nanoseconds latency;

	if (output_audio)
		latency = output_audio->Latency();
	else
		latency = std::chrono::nanoseconds(0);

	event->timestamp = AUDIO::AudioManager::GetInstance().GetLastCallbackTime() + latency;

	Logger::Log(trace,
	            "Event received: type={} latency={}ns",
	            static_cast<int>(event->type),
	            std::chrono::duration_cast<std::chrono::nanoseconds>(latency).count());

	frame_dispatcher.PushFrame(std::move(event));
}

void HybridSession::SendAudio(const std::unique_ptr<StationAudioFrame> &audio_frame)
{
	if (!SDL_PutAudioStreamData(output_audio->stream.get(),
	                            audio_frame->data,
	                            audio_frame->size * sizeof(int16_t)))
	{
		Logger::Log(err, "Failed to queue audio data: {}", SDL_GetError());
	}
}

void HybridSession::Process()
{
	frame_dispatcher.Tick(
		[this](const std::unique_ptr<EventData> &frame)
		{
			const auto ptr = frame.get();

			switch (frame->type)
			{
				case PROGRAM_CHANGE:
				{
					OnProgramStateChange(dynamic_cast<const ProgramChangeFrame *>(ptr));
					break;
				}
				case STATION_ID3:
				{
					OnId3Update(dynamic_cast<const StationId3Frame *>(ptr));
					break;
				}
				case STATION_PROGRAM:
				{
					OnProgramUpdate(dynamic_cast<const StationProgramFrame *>(ptr));
					break;
				}
				default:
					break;
			}
		});
}

void HybridSession::SendStation(const StationIdentity &identity)
{
	identity_ = identity;
	FetchStationImage();
	UpdateProgramName();
}

void HybridSession::FetchStationImage()
{
	const MetadataQuery query{
		identity_.country_code,
		identity_.name,
		MimeQuery::StationLogo,
		program_.id,
	};
	program_.station_logo = external_service_->FetchImageAsync(query);
}

void HybridSession::FetchPrimaryImage(const int xhdr)
{
	const MetadataQuery query{
		identity_.country_code,
		identity_.name,
		MimeQuery::PrimaryLogo,
		program_.id,
		xhdr
	};

	program_.primary_image = external_service_->FetchImageAsync(query);
}

void HybridSession::UpdateProgramName()
{
	if (identity_.name.empty())
		return;

	program_.formatted_name = fmt::format(
		"{}-HD{}",
		identity_.name,
		NRSC5::FriendlyProgramId(program_.id)
	);
}

void HybridSession::OnProgramUpdate(const StationProgramFrame *ptr)
{
	program_.type = ptr->type;
	program_.id = ptr->id;

	UpdateProgramName();
}

void HybridSession::OnProgramStateChange(const ProgramChangeFrame *frame)
{
	program_ = frame->state;

	FetchStationImage();

	if (!program_.id3.xhdr.Empty())
		FetchPrimaryImage(program_.id3.xhdr.lot);
}

void HybridSession::OnId3Update(const StationId3Frame *frame)
{
	if (program_.id3.xhdr.lot != frame->id3.xhdr.lot)
		FetchPrimaryImage(frame->id3.xhdr.lot);

	program_.id3 = frame->id3;
}
