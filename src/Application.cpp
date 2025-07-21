//
// Created by TheDaChicken on 7/13/2025.
//

#include "Application.h"

#include <imgui_internal.h>

#include "gui/TextureAsset.h"

Application::Application()
	: assets_manager_(image_manager_)
{
	if (m_glfwImpl.Initialize() != 0)
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}

	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

	std::filesystem::path font_path;
	GUI::AssetsManager::GetAsset("fonts/OpenSans-VariableFont.ttf", font_path);

	font_ = io.Fonts->AddFontFromFileTTF(font_path.string().c_str(),
	                                     16.0f,
	                                     nullptr,
	                                     io.Fonts->GetGlyphRangesDefault());

	// Setup Dear ImGui style
	ImGui::StyleColorsLight();
	//ImGui::StyleColorsDark();

	const float main_scale = m_glfwImpl.GetScaling();

	// Setup scaling
	ImGuiStyle &style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);
	// Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;
	// Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

	m_glfwImpl.SetCallback(this);
	m_glfwImpl.CreateWindow(1180, 576, "nrsc5_gui");

	GUI::Image black_navigation;
	GUI::Image blue_navigation;
	GUI::Image play;
	GUI::Image paused;
	GUI::Image black_radio;
	GUI::Image blue_radio;

	// Load an image from file
	assets_manager_.LoadImageAsset("buttons/black/Navigation.svg", black_navigation);
	assets_manager_.LoadImageAsset("buttons/BlueNavigation.svg", blue_navigation);
	assets_manager_.LoadImageAsset("buttons/black/Play.svg", play);
	assets_manager_.LoadImageAsset("buttons/black/Pause.svg", paused);
	assets_manager_.LoadImageAsset("buttons/black/Note.svg", black_radio);
	assets_manager_.LoadImageAsset("buttons/BlueNote.svg", blue_radio);

	GUI::CircleImageButton button2("Play");

	button2.SetOffImage(std::make_shared<GUI::TextureAsset>(play));
	button2.SetOnImage(std::make_shared<GUI::TextureAsset>(paused));

	button_group_.buttons_.emplace_back(button2);

	GUI::CircleImageButton radio("Radio");

	radio.SetOffImage(std::make_shared<GUI::TextureAsset>(black_radio));
	radio.SetOnImage(std::make_shared<GUI::TextureAsset>(blue_radio));

	button_group_.buttons_.emplace_back(radio);

	GUI::CircleImageButton navigation("Navigation");

	navigation.SetOffImage(std::make_shared<GUI::TextureAsset>(black_navigation));
	navigation.SetOnImage(std::make_shared<GUI::TextureAsset>(blue_navigation));

	button_group_.buttons_.emplace_back(navigation);
}

void Application::Run()
{
	m_glfwImpl.EventLoop();
}

void Application::Render()
{
	//ImGui::ShowDemoWindow();

	const ImGuiViewport *viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::Begin("Begin World", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

	// Touchscreen buttons on left.
	button_group_.Render();

	ImGui::SameLine();

	// Options and content area

	constexpr auto size = ImVec2{300, 70};

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	const float child_width = size.x + ImGui::GetStyle().WindowPadding.x * 2 + ImGui::GetStyle().FramePadding.x * 2;

	ImGui::BeginChild("Header", ImVec2(child_width, 0), ImGuiChildFlags_FrameStyle);

	// Show we are the radio section
	ImGui::PushFont(font_, ImGui::GetStyle().FontSizeBase * 2.0f);
	ImGui::Dummy(ImVec2(0, 12));
	ImGui::TextAligned(0.2f, size.x, "Radio");
	ImGui::Dummy(ImVec2(0, 12));

	ImGui::PopFont();

	ImGui::Button("fart", size);

	ImGui::EndChild();

	ImGui::PopStyleVar(3);

	ImGui::End();

	ImGui::PopStyleVar();
}
