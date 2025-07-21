//
// Created by TheDaChicken on 7/13/2025.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gui/AssetsManager.h"
#include "gui/ImageManager.h"
#include "gui/backends/GlfwImpl.h"
#include "gui/imgui/Imgui.h"
#include "gui/widgets/ButtonGroup.h"

class Application final : public GlfwImpl::Callback {
	public:
		Application();

		void Run();
	private:
		void Render() override;

		ScopedImgui m_imgui;
		GlfwImpl m_glfwImpl;

		GUI::ImageManager image_manager_;
		GUI::AssetsManager assets_manager_;
		GUI::ButtonGroup button_group_;

		ImFont *font_ = nullptr;
};

#endif //MAINWINDOW_H
