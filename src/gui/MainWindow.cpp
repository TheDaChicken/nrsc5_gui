//
// Created by TheDaChicken on 9/5/2025.
//

#include "MainWindow.h"

#include <SDL3/SDL_timer.h>

#include "panels/DockAudioPanel.h"
#include "view/SettingsView.h"
#include "widgets/IconButton.h"

bool MainWindow::InitWindow(const std::shared_ptr<GUI::SDLPlatformContext> &platform)
{
	m_window_ = platform->CreateWindow();
	if (!m_window_)
	{
		Logger::Log(err, "Failed to create Window object");
		return false;
	}

	if (!m_window_->CreateWindow(
		"nrsc5_gui",
		1180,
		576))
	{
		Logger::Log(err, "Failed to create window");
		return false;
	}

	platform->SetDefaultTheme();

	theme_manager_ = std::make_unique<ThemeManager>(platform);
	if (!theme_manager_->Init())
	{
		Logger::Log(err, "Failed to initialize theme manager");
		return false;
	}

	//sdr_host_ = std::make_shared<SDRHost>();
	session_ = std::make_shared<UISession>();
	dock_input_panel_ = std::make_shared<DockInputPanel>(app_context_->rc);

	auto settings = std::make_unique<SettingsView>();

	settings->AddView("Input", dock_input_panel_);
	settings->AddView("Audio", std::make_unique<DockAudioPanel>());

	apps_list_[2].view = std::move(settings);
	apps_list_[0].view = std::make_unique<RadioView>(
		session_);

	if (!session_->OpenAudio())
	{
		Logger::Log(err, "Failed to open audio on player");
		return false;
	}

	context_.selected_host = 0;
	context_.app = app_context_;
	return true;
}

void MainWindow::Render()
{
	if (m_window_->IsMinimized())
	{
		SDL_Delay(10);
		return;
	}

	session_->Process();
	m_window_->BeginFrame();

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

	m_window_->SubmitFrame();
}

void MainWindow::ProcessEvent(const SDL_Event *event)
{
	// TODO: Why is this here?
	ImGui_ImplSDL3_ProcessEvent(event);
}

void MainWindow::RenderLayout()
{
	const Theme &theme = theme_manager_->GetCurrentTheme();

	RenderButtons(theme);

	context_.theme = theme;

	SessionUi &session = context_.session;

	//context_.app->rc->PollDevice(session);

	ImGui::SameLine(0.0f, 0.0f);

	RenderCenter(context_);
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

		SessionUi &session = context_.session;
		const bool checked = session.running;
		const GUI::IconButton play_button = {
			"PlayButton",
			checked
				? theme.GetIcon(IconType::Paused)
				: theme.GetIcon(IconType::Play),
		};
		if (play_button.Render(checked))
		{
			// TODO: check session_id.
			if (!checked)
			{
				//context_.app->rc->StartSession(session);
			}
			else
			{
				//context_.app->rc->StopSession(session);
			}
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

void MainWindow::RenderCenter(RenderContext &theme) const
{
	if (auto &view = apps_list_[left_buttons_id].view)
	{
		view->Render(theme);
	}
}
