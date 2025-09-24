//
// Created by TheDaChicken on 9/2/2025.
//

#include "Application.h"

Application::Application()
	: lot_manager_(sql_manager), favorite_list_(sql_manager)
{
}

Application::~Application()
{
}

SDL_AppResult Application::Initialize()
{
	gpu_context_ = std::make_shared<GPU::GPUContext>();
	if (!gpu_context_->OpenDevice())
	{
		Logger::Log(err, "Failed to open GPU device");
		return SDL_APP_FAILURE;
	}

	image_manager_ = std::make_unique<ImageManager>(gpu_context_->GetUploader());

	const auto ret = sql_manager.Open("database.db", 3);
	if (!ret)
	{
		Logger::Log(err, "Failed to open database");
		return SDL_APP_FAILURE;
	}

	lot_manager_.SetImageFolder("HDImages");

	external_service_ = std::make_shared<HybridExternal>(image_manager_, sql_manager);
	main_window_ = std::make_unique<MainWindow>(
		external_service_,
		input_,
		favorite_list_);

	input_.on_lot = [this](const NRSC5::Lot &lot)
	{
		StationIdentity identity = input_.Sessions().GetIdentity();

		ThreadPool::GetInstance().QueueJob([lot, this, identity]
		{
			if (lot.component.programId)
				external_service_->ReceivedLotImage(identity, lot);

			if (!lot_manager_.LotReceived(identity, lot))
			{
				Logger::Log(err, "Failed to process received lot");
			}
		});
	};

	if (main_window_->InitWindow(gpu_context_) != SDL_APP_CONTINUE)
	{
		Logger::Log(err, "Failed to create main window");
		return SDL_APP_FAILURE;
	}

	favorite_list_.Update();
	return SDL_APP_CONTINUE;
}

void Application::Render() const
{
	image_manager_->Process();

	main_window_->Render();
}

void Application::ProcessEvent(const SDL_Event *event) const
{
	main_window_->ProcessEvent(event);
}
