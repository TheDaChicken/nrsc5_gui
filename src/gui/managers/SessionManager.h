//
// Created by TheDaChicken on 11/29/2025.
//

#ifndef NRSC5_GUI_SESSIONMANAGER_H
#define NRSC5_GUI_SESSIONMANAGER_H

#include <unordered_map>

#include "gui/controllers/Events.h"
#include "hybrid/HybridSession.h"
#include "input/RadioInput.h"

class SessionsManager
{
	public:
		SessionsManager()
			: session_id_(0)
		{
		}

		SessionId CreateSession()
		{
			std::scoped_lock lock(mutex_);
			auto session_id = session_id_++;
			sessions_.emplace(session_id, std::make_shared<HybridSession>());
			return session_id;
		}

		std::shared_ptr<HybridSession> GetSession(const SessionId session_id) const
		{
			std::scoped_lock lock(mutex_);
			const auto iter = sessions_.find(session_id);
			if (iter == sessions_.end())
				return nullptr;
			return iter->second;
		}

		void CreateSession(const PortSDR::Device &device);
	private:
		SessionId session_id_;
		mutable std::mutex mutex_;

		std::unordered_map<SessionId, std::shared_ptr<HybridSession> > sessions_;
};

#endif //NRSC5_GUI_SESSIONMANAGER_H
