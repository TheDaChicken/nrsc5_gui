//
// Created by TheDaChicken on 7/13/2025.
//

#define SDL_MAIN_USE_CALLBACKS

#include <backends/imgui_impl_sdl3.h>
#include <SDL3/SDL_main.h>

#include "Application.h"
#include "audio/AudioManager.h"
#include "utils/Log.h"
#include "utils/ThreadPool.h"

SDL_AppResult SDL_Fail()
{
	Logger::Log(err, "Error {}", SDL_GetError());
	return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
	// init the library, here we make a window so we only need the Video capabilities.
	if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD))
		return SDL_Fail();

	ThreadPool::GetInstance().Start(2);
	if (!AUDIO::AudioManager::GetInstance().Initialize())
	{
		Logger::Log(err, "Failed to initialize audio manager");
		return SDL_Fail();
	}

	const auto app = new Application();
	if (app->Initialize() != SDL_APP_CONTINUE)
	{
		delete app;
		return SDL_APP_FAILURE;
	}

	*appstate = app;
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
	auto *backend = static_cast<Application *>(appstate);
	if (backend->IsQuit())
	{
		return SDL_APP_SUCCESS;
	}

	backend->Render();
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	auto *app = static_cast<Application *>(appstate);

	if (event->type == SDL_EVENT_QUIT)
	{
		app->SetQuit(SDL_APP_SUCCESS);
	}

	app->ProcessEvent(event);
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	if (const auto *application = static_cast<Application *>(appstate))
		delete application;

	AUDIO::AudioManager::GetInstance().Uninitialize();

	SDL_Quit();
}
