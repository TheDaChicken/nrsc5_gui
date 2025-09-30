//
// Created by TheDaChicken on 9/5/2025.
//

#include "MainWindow.h"

#include <SDL3/SDL_timer.h>

SDL_AppResult MainWindow::InitWindow(const std::shared_ptr<GPU::GPUContext> &gpu_device)
{
	if (!sdl_imgui.CreateWindow(
		gpu_device,
		"nrsc5_gui",
		1180,
		576))
	{
		Logger::Log(err, "Failed to create main window");
		return SDL_APP_FAILURE;
	}

	SDL_SetPreferredSystemTheme(SDL_SYSTEM_THEME_LIGHT);

	theme_manager_ = std::make_unique<ThemeManager>(gpu_device->GetUploader());
	session_ = std::make_shared<HybridSession>(external_);

	if (!theme_manager_->Init())
	{
		Logger::Log(err, "Failed to initialize theme manager");
		return SDL_APP_FAILURE;
	}

	if (!session_->OpenAudio())
	{
		Logger::Log(err, "Failed to open audio on player");
		return SDL_APP_FAILURE;
	}

	input_.Sessions().Subscribe(session_);

	apps_list_[0].view = std::make_unique<RadioView>(
		external_,
		favorite_list_,
		input_,
		session_);
	apps_list_[2].view = std::make_unique<SettingsView>(
		dock_input_panel_,
		dock_audio_panel_);
	return SDL_APP_CONTINUE;
}

void MainWindow::Render()
{
	if (sdl_imgui.isMinimized())
	{
		SDL_Delay(10);
		return;
	}

	session_->Process();
	sdl_imgui.BeginFrame();

	ImGuiWindowFlags flags = 0;
	const ImGuiViewport *viewport = ImGui::GetMainViewport();

	// ImGui::SetNextWindowPos(viewport->Pos);
	// ImGui::SetNextWindowSize(viewport->Size);
	// flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("MainWindow",
	             nullptr,
	             flags);
	ImGui::PopStyleVar();
	RenderLayout();
	ImGui::End();

	// ImGui::ShowMetricsWindow();
	ImGui::ShowDemoWindow();

	sdl_imgui.SubmitFrame();
}

void MainWindow::ProcessEvent(const SDL_Event *event)
{
	ImGui_ImplSDL3_ProcessEvent(event);
}

void MainWindow::RenderLayout()
{
	const Theme &theme = theme_manager_->GetCurrentTheme();

	RenderButtons(theme);

	ImGui::SameLine(0.0f, 0.0f);

	RenderCenter(theme);
}

void MainWindow::RenderButtons(const Theme &theme)
{
	ImGui::PushFont(nullptr, 35);
	ImGui::PushStyleVarY(ImGuiStyleVar_WindowPadding, 40.0f);

	const float max_size =
			ImGui::GetFontSize() * 2.0f + ImGui::GetFontSize() * 2.0f;

	ImGui::BeginChild("Buttons",
	                  ImVec2(max_size, 0),
	                  ImGuiChildFlags_Borders);
	{
		ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 40.0f);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
			(ImGui::GetContentRegionAvail().x - ImGui::GetFontSize() * 2) / 2.0f);
		ImGui::BeginGroup();

		const bool checked = input_.GetStatus() == Playing;
		const GUI::IconButton play_button = {
			"PlayButton",
			checked
				? theme.GetIcon(IconType::Paused)
				: theme.GetIcon(IconType::Play),
		};
		if (play_button.Render(checked))
		{
			if (!checked)
				input_.Play();
			else
				input_.Stop();
		}

		for (int i = 0; i < apps_list_.size(); i++)
		{
			const auto &app = apps_list_[i];
			GUI::IconButton button{
				app.name,
				theme.GetIcon(app.type),
			};

			if (button.Render(left_buttons_id == i))
				left_buttons_id = i;
		}

		ImGui::EndGroup();
		ImGui::PopStyleVar(1);
	}
	ImGui::EndChild();

	ImGui::PopStyleVar();
	ImGui::PopFont();
}

void MainWindow::RenderCenter(const Theme &theme) const
{
	if (auto &view = apps_list_[left_buttons_id].view)
	{
		view->Render(theme);
	}
}
