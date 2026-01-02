//
// Created by TheDaChicken on 9/2/2025.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL3/SDL_init.h>

#include "AppState.h"
#include "gui/MainWindow.h"
#include "gui/platform/sdl/SDLPlatformContext.h"
#include "models/FavoriteList.h"
#include "nrsc5/LotManager.h"
#include "sql/DatabaseManager.h"

class Application final
{
	public:
		Application();
		~Application();

		bool Initialize();

		[[nodiscard]] bool IsQuit() const
		{
			return quit == SDL_APP_SUCCESS;
		}

		void SetQuit(const SDL_AppResult result)
		{
			quit = result;
		}

		void Render() const;
		void ProcessEvent(const SDL_Event *event) const;
	private:
		SDL_AppResult quit = SDL_APP_CONTINUE;

		std::shared_ptr<GUI::SDLPlatformContext> gpu_context_;
		std::shared_ptr<GUI::ImageManager> image_manager_;
		std::shared_ptr<AppContext> app_context_;

		//std::shared_ptr<HybridExternal> external_service_;
		//std::shared_ptr<HybridHost> hybrid_host_;

		DatabaseManager sql_manager;

		LotManager lot_manager_;
		FavoriteList favorite_list_;

		std::unique_ptr<MainWindow> main_window_{};
};

#endif //APPLICATION_H
