//
// Created by TheDaChicken on 9/18/2025.
//

#ifndef IWINDOW_H
#define IWINDOW_H

#include <memory>
#include <string>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlgpu3.h>
#include <SDL3/SDL_init.h>

using SDLWINDOW_PTR = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;

namespace GUI
{
class SDLWindow
{
	public:
		explicit SDLWindow(const std::shared_ptr<SDL_GPUDevice>& device)
			: m_gpu(device), m_window(nullptr, SDL_DestroyWindow)
		{
		}

		~SDLWindow()
		{
			DestroyWindow();
		}

		bool CreateWindow(const std::string &name, int width, int height);
		void DestroyWindow();

		void BeginFrame();
		void SubmitFrame() const;

		bool IsMinimized() const
		{
			return (SDL_GetWindowFlags(m_window.get()) & SDL_WINDOW_MINIMIZED) != 0;
		}

		int32_t WindowID()
		{
			return SDL_GetWindowID(m_window.get());
		}

	protected:
		static constexpr auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		std::shared_ptr<SDL_GPUDevice> m_gpu;
		SDLWINDOW_PTR m_window;
};
} // namespace GPU

#endif //IWINDOW_H
