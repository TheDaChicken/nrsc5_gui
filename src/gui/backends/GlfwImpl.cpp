//
// Created by TheDaChicken on 7/13/2025.
//

#include "GlfwImpl.h"

#include <cstdio>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

static void window_refresh_callback(GLFWwindow *window)
{
	auto *impl = static_cast<GlfwImpl *>(glfwGetWindowUserPointer(window));

	impl->Render();

	glfwSwapBuffers(window);
	glFinish(); // important, this waits until rendering result is actually visible, thus making resizing less ugly
}

static void glfw_error_callback(int error, const char *description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

GlfwImpl::GlfwImpl()
	: m_window(nullptr), m_callback(nullptr)
{
}

GlfwImpl::~GlfwImpl()
{
	if (m_window == nullptr)
		return;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

int GlfwImpl::Initialize()
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return -1;

	return 0;
}

int GlfwImpl::CreateWindow(int width, int height, const char *title)
{
#if defined(__APPLE__)
	// GL 3.2 + GLSL 150
	auto glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	auto glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

	m_window = glfwCreateWindow(
		static_cast<int>(static_cast<float>(width) * GetScaling()),
		static_cast<int>(static_cast<float>(height) * GetScaling()),
		title,
		nullptr,
		nullptr);

	if (m_window == nullptr)
		return -1;

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // Enable vsync

	glfwSetWindowUserPointer(m_window, this);
	glfwSetWindowRefreshCallback(m_window, &window_refresh_callback);

	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	return 0;
}

ImTextureID GlfwImpl::LoadTexture(const GUI::Image &image)
{
	const int color_type = ColorType(image.color_type);
	const int bit_depth = BitDepth(image.color_type, image.bit_depth);
	if (color_type == 0 || bit_depth == 0)
	{
		return 0;
	}

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Upload pixels into texture
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D,
	             0,
	             bit_depth,
	             image.width,
	             image.height,
	             0,
	             color_type,
	             GL_UNSIGNED_BYTE,
	             image.data.data());

	return static_cast<intptr_t>(image_texture);
}

void GlfwImpl::UnloadTexture(ImTextureID texture)
{
	const auto image_texture = static_cast<GLuint>(reinterpret_cast<uintptr_t>(texture));
	glDeleteTextures(1, &image_texture);
}

void GlfwImpl::EventLoop() const
{
	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();
		if (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0)
		{
			ImGui_ImplGlfw_Sleep(10);
			continue;
		}

		Render();

		glfwSwapBuffers(m_window);
	}
}
void GlfwImpl::Render() const
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (m_callback)
	{
		m_callback->Render();
	}
	ImGui::Render();

	int display_w, display_h;
	glfwGetFramebufferSize(m_window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x * clear_color.w,
				 clear_color.y * clear_color.w,
				 clear_color.z * clear_color.w,
				 clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
