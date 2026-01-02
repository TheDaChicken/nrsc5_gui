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

bool Application::Initialize()
{
	UTILS::ThreadPool::GetInstance().Start(2);

	gpu_context_ = std::make_shared<GUI::SDLPlatformContext>();
	if (!gpu_context_->OpenDevice())
	{
		Logger::Log(err, "Failed to open GPU device");
		return false;
	}

	image_manager_ = std::make_unique<GUI::ImageManager>(gpu_context_);

	const auto ret = sql_manager.Open("database.db", 3);
	if (!ret)
	{
		Logger::Log(err, "Failed to open database");
		return false;
	}

	app_context_ = std::make_shared<AppContext>();
	app_context_->am = std::make_unique<AUDIO::SDLAudioManager>();
	app_context_->fc = std::make_unique<FavoritesController>(favorite_list_);
	app_context_->rc = std::make_unique<SDRController>();

	if (!app_context_->am->OpenAudioDevice(nullptr))
	{
		Logger::Log(err, "Failed to open default audio device");
		return false;
	}

	main_window_ = std::make_unique<MainWindow>(
		app_context_);

	if (!main_window_->InitWindow(gpu_context_))
	{
		Logger::Log(err, "Failed to create main window");
		return false;
	}

	favorite_list_.Update();
	return true;
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
