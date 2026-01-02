//
// Created by TheDaChicken on 9/5/2025.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "AppState.h"
#include "platform/sdl/SDLWindow.h"
#include "platform/sdl/SDLPlatformContext.h"
#include "panels/DockInputPanel.h"
#include "view/radio/RadioView.h"

class MainWindow final
{
	public:
		explicit MainWindow(
			const std::shared_ptr<AppContext> &app_context
		)
			: context_(), app_context_(app_context)
		{
		}

		~MainWindow()
		{
		}

		bool InitWindow(const std::shared_ptr<GUI::SDLPlatformContext> &platform);

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
		void RenderCenter(RenderContext &theme) const;

		std::unique_ptr<GUI::SDLWindow> m_window_;

		int left_buttons_id = 0;

		RenderContext context_;

		//std::shared_ptr<HybridExternal> external_;
		const std::shared_ptr<AppContext> app_context_;
		std::shared_ptr<UISession> session_;
		std::shared_ptr<ThemeManager> theme_manager_;
		//std::shared_ptr<SDRHost> sdr_host_;

		std::shared_ptr<DockInputPanel> dock_input_panel_;

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
