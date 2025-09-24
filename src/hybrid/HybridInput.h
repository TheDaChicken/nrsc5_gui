//
// Created by TheDaChicken on 8/27/2025.
//

#ifndef HYBRIDPLAYER_H
#define HYBRIDPLAYER_H

#include "HybridController.h"
#include "HybridSessionManager.h"
#include "external/HybridExternal.h"
#include "audio/AudioManager.h"
#include "input/RadioInput.h"

enum RadioStatus
{
	Loading,
	Idle,
	Starting,
	Stopping,
	Playing,
	Error
};

class HybridInput
{
	public:
		std::function<void(const NRSC5::Lot &lot)> on_lot;

		explicit HybridInput();

		void OpenSDR(const std::shared_ptr<PortSDR::Device> &device);
		void OpenFile(const std::string &path);

		void SetChannel(const Station &station);
		void Play();
		void Stop();

		void NextChannel(const std::shared_ptr<HybridSession> &session) const;
		void PreviousChannel(const std::shared_ptr<HybridSession> &session) const;

		RadioStatus GetStatus() const
		{
			return status_;
		}

		HybridSessionManager &Sessions()
		{
			return session_manager_;
		}

		const HybridSessionManager &Sessions() const
		{
			return session_manager_;
		}

		std::shared_ptr<RadioInput> &GetInput()
		{
			return input_;
		}

	private:
		std::shared_ptr<HybridController> controller_;
		HybridSessionManager session_manager_;

		std::shared_ptr<RadioInput> input_;

		std::atomic<RadioStatus> status_{Idle};
};

#endif //HYBRIDPLAYER_H
