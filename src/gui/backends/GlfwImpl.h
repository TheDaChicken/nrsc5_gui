//
// Created by TheDaChicken on 7/13/2025.
//

#ifndef GLFWIMPL_H
#define GLFWIMPL_H

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "../image/Image.h"

class GlfwImpl
{
	public:
		class Callback
		{
			public:
				virtual ~Callback() = default;

				virtual void Render() = 0;
		};

		GlfwImpl();
		~GlfwImpl();

		int Initialize();
		int CreateWindow(int width, int height, const char *title);

		static ImTextureID LoadTexture(const GUI::Image &image);
		static void UnloadTexture(ImTextureID texture);

		void EventLoop() const;

		[[nodiscard]] float GetScaling() const
		{
			return ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
		}

		void SetCallback(Callback *callback)
		{
			m_callback = callback;
		}

		void Render() const;

	private:
		static constexpr auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		static int ColorType(const GUI::Image::ColorType type)
		{
			switch (type)
			{
				case GUI::Image::ColorType::RGB:
					return GL_RGB;
				case GUI::Image::ColorType::RGBA:
					return GL_RGBA;
				default:
					return 0;
			}
		}

		static int BitDepth(const GUI::Image::ColorType type, const GUI::Image::BitDepth bitdepth)
		{
			switch (bitdepth)
			{
				case GUI::Image::BitDepth::Bit2:
				{
					if (type == GUI::Image::ColorType::RGBA)
						return GL_RGBA2;
					break;
				}
				case GUI::Image::BitDepth::Bit4:
				{
					if (type == GUI::Image::ColorType::RGBA)
						return GL_RGBA4;
					break;
				}
				case GUI::Image::BitDepth::Bit8:
				{
					if (type == GUI::Image::ColorType::RGBA)
						return GL_RGBA8;
					if (type == GUI::Image::ColorType::RGB)
						return GL_RGB8;
					if (type == GUI::Image::ColorType::Grayscale)
						return GL_LUMINANCE8;
					break;
				}
				case GUI::Image::BitDepth::Bit16:
				{
					if (type == GUI::Image::ColorType::RGBA)
						return GL_RGBA16;
					if (type == GUI::Image::ColorType::RGB)
						return GL_RGB16;
					if (type == GUI::Image::ColorType::Grayscale)
						return GL_LUMINANCE16;
					break;
				}
				default:
					return 0;
			}
			return 0;
		}

		GLFWwindow *m_window;
		Callback *m_callback;
};

#endif //GLFWIMPL_H
