//
// Created by TheDaChicken on 11/22/2025.
//

#ifndef NRSC5_GUI_INPUTCONTROLLER_H
#define NRSC5_GUI_INPUTCONTROLLER_H

#include <PortSDR.h>

#include "Events.h"
#include "../../input/SDRHost.h"
#include "gui/panels/DeviceList.h"
#include "gui/managers/SessionManager.h"

enum DeviceStatus : int
{
	OPEN_LOADING = 1,
	OPEN_SUCCESS = 2,
	OPEN_FAILED = 3
};

class SDRController
{
	public:
		explicit SDRController();
		~SDRController();

		HybridSession& GetSession(const SDRInfo &dev)
		{
			const auto iter = sessions_.find(dev);
			// if (iter == sessions_.end())
			// 	TODO: Fix me
			return iter->second;
		}

		std::shared_ptr<HybridSession> OpenSDR(const SDRInfo &device);

	private:
		SDRHost host_;

		std::map<SDRInfo, HybridSession > sessions_;
		std::mutex mutex_;
};

#endif //NRSC5_GUI_INPUTCONTROLLER_H
