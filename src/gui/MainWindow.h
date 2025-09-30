//
// Created by TheDaChicken on 9/5/2025.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <SDL3/SDL_init.h>

#include "SDLWindow.h"
#include "panels/DockAudioPanel.h"
#include "panels/DockFavoritesPanel.h"
#include "panels/DockInputPanel.h"
#include "view/radio/RadioView.h"
#include "view/SettingsView.h"
#include "widgets/IconButton.h"
#include "wrappers/GPUContext.h"

class MainWindow final
{
	public:
		MainWindow(
			const std::shared_ptr<HybridExternal> &external,
			HybridInput &input,
			FavoriteList &favorites
		)
			: favorite_list_(favorites),
			  input_(input),
			  external_(external),
			  dock_input_panel_(input)
		{
		}

		~MainWindow()
		{
			input_.Sessions().Unsubscribe(session_);
		}

		SDL_AppResult InitWindow(const std::shared_ptr<GPU::GPUContext> &gpu_device);

		void Render();
		void ProcessEvent(const SDL_Event *event);

	private:
		struct AppsList // TODO: Whats a better name?
		{
			IconType type;
			std::string name;
			std::unique_ptr<IView> view;
		};

		void RenderLayout();
		void RenderButtons(const Theme &theme);
		void RenderNavigation(const Theme &theme);
		void RenderCenter(const Theme &theme) const;

		SDLWindow sdl_imgui;

		int left_buttons_id = 0;

		FavoriteList &favorite_list_;
		HybridInput &input_;

		std::shared_ptr<HybridExternal> external_;
		std::shared_ptr<HybridSession> session_;
		std::shared_ptr<ThemeManager> theme_manager_;

		DockInputPanel dock_input_panel_;
		DockAudioPanel dock_audio_panel_;

		std::array<AppsList, 3> apps_list_ =
		{
			{
				{IconType::Note, "Radio"},
				{IconType::Navigation, "Maps"},
				{IconType::Settings, "Settings"}
			}
		};
};

#endif //MAINWINDOW_H
