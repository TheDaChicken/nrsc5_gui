//
// Created by TheDaChicken on 8/4/2025.
//

#ifndef DOCKINPUTPANEL_H
#define DOCKINPUTPANEL_H

#include "AppState.h"
#include "SDRSettings.h"

class DockInputPanel final : public IView
{
	public:
		explicit DockInputPanel(const std::shared_ptr<SDRController> &rc);

		void RefreshDevice();
		void Render(RenderContext &context) override;
	private:
		struct Devices
		{
			DeviceHolder device;
			int unique_id_;

			UTILS::Future<int, void> open_device_future;
		} m_context;

		SDRSettings panel_;
		InputDevicesState state_;
		SDRHost api_;
		UTILS::Future<void, int> devices_future_;

		const std::shared_ptr<SDRController> rc_;
};

#endif //DOCKINPUTPANEL_H
