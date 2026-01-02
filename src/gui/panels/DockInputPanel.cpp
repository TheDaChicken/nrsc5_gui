//
// Created by TheDaChicken on 8/4/2025.
//

#include "DockInputPanel.h"

DockInputPanel::DockInputPanel(const std::shared_ptr<SDRController> &rc)
	: rc_(rc)
{

}

void DockInputPanel::RefreshDevice()
{
	static float timer = 0.0f;

	timer += ImGui::GetIO().DeltaTime;

	if (timer > 1.0f)
	{
		if (devices_future_.IsValid() && !devices_future_.IsReady())
		{
			Logger::Log(debug, "Still refreshing device list.");
			return;
		}

		UTILS::Promise<void, int> promise;
		devices_future_ = promise.GetFuture();

		UTILS::ThreadPool::GetInstance().QueueJob([this, p = std::move(promise)]() mutable
		{
			if (p.IsFinished())
				return;

			api_.UpdateDeviceList();

			p.SetValue();
		});

		timer = 0.0f;
	}
}

void DockInputPanel::Render(RenderContext &context)
{
	ImGui::PushFont(
		context.theme.GetFont(FontType::Semibold),
		context.theme.font_medium_size);

	RefreshDevice();

	state_.OpenDevice = [this, &context](const DeviceUi& device) mutable
	{
		context.session.device = device;
		rc_->OpenSDR(device);
	};
	// TODO: this seems like this needs to be set every single time somewhere?
	//  How can we keep device in sync?
	state_.selected_device = context.session.device;

	DeviceList::Render(state_);

	ImGui::PopFont();
}
