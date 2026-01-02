//
// Created by TheDaChicken on 9/18/2025.
//

#include "SDLWindow.h"

#include "utils/Log.h"

bool GUI::SDLWindow::CreateWindow(
	const std::string &name,
	const int width,
	const int height)
{
	// Create SDL window graphics context
	const float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

	constexpr SDL_WindowFlags window_flags =
			SDL_WINDOW_RESIZABLE |
			SDL_WINDOW_HIDDEN |
			SDL_WINDOW_HIGH_PIXEL_DENSITY;

	auto new_window = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>(
		SDL_CreateWindow(
			name.c_str(),
			static_cast<int>(width * main_scale),
			static_cast<int>(height * main_scale),
			window_flags),
		SDL_DestroyWindow);
	if (!new_window)
	{
		Logger::Log(err, "Failed to create main window {}", SDL_GetError());
		return false;
	}

	// Claim window for GPU Device
	if (!SDL_ClaimWindowForGPUDevice(m_gpu.get(), new_window.get()))
	{
		Logger::Log(err, "Failed to claim window for GPU device {}", SDL_GetError());
		return false;
	}

	if (!SDL_SetGPUSwapchainParameters(
		m_gpu.get(),
		new_window.get(),
		SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
		SDL_GPU_PRESENTMODE_VSYNC))
	{
		Logger::Log(err, "Failed to set swapchain parameters {}", SDL_GetError());
		return false;
	}

	SDL_SetWindowPosition(new_window.get(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(new_window.get());

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForSDLGPU(new_window.get());
	ImGui_ImplSDLGPU3_InitInfo init_info = {};
	init_info.Device = m_gpu.get();
	init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(
		m_gpu.get(),
		new_window.get());
	init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1; // Only used in multi-viewports mode.
	ImGui_ImplSDLGPU3_Init(&init_info);

	m_window = std::move(new_window);
	return true;
}

void GUI::SDLWindow::DestroyWindow()
{
	if (!m_window)
		return;

	ImGui_ImplSDL3_Shutdown();
	ImGui_ImplSDLGPU3_Shutdown();
	ImGui::DestroyContext();

	SDL_ReleaseWindowFromGPUDevice(m_gpu.get(), m_window.get());
	m_window.reset();
}

void GUI::SDLWindow::BeginFrame()
{
	// Start the Dear ImGui frame
	ImGui_ImplSDLGPU3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void GUI::SDLWindow::SubmitFrame() const
{
	// Rendering
	ImGui::Render();
	ImDrawData *draw_data = ImGui::GetDrawData();
	const bool is_minimized = draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f;

	SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(m_gpu.get());
	// Acquire a GPU command buffer

	SDL_GPUTexture *swapchain_texture;
	SDL_WaitAndAcquireGPUSwapchainTexture(
		command_buffer,
		m_window.get(),
		&swapchain_texture,
		nullptr,
		nullptr);

	// Acquire a swapchain texture
	if (swapchain_texture != nullptr && !is_minimized)
	{
		// This is mandatory: call ImGui_ImplSDLGPU3_PrepareDrawData() to upload the vertex/index buffer!
		ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, command_buffer);

		// Setup and start a render pass
		SDL_GPUColorTargetInfo target_info = {};
		target_info.texture = swapchain_texture;
		target_info.clear_color = SDL_FColor{clear_color.x, clear_color.y, clear_color.z, clear_color.w};
		target_info.load_op = SDL_GPU_LOADOP_CLEAR;
		target_info.store_op = SDL_GPU_STOREOP_STORE;
		target_info.mip_level = 0;
		target_info.layer_or_depth_plane = 0;
		target_info.cycle = false;
		SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(command_buffer, &target_info, 1, nullptr);

		// Render ImGui
		ImGui_ImplSDLGPU3_RenderDrawData(draw_data, command_buffer, render_pass);

		SDL_EndGPURenderPass(render_pass);
	}

	// Submit the command buffer
	SDL_SubmitGPUCommandBuffer(command_buffer);
}
