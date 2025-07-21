//
// Created by TheDaChicken on 7/13/2025.
//

#include "ImageButton.h"

#include <imgui.h>

#include <utility>

GUI::CircleImageButton::CircleImageButton(std::string label)
	: m_label(std::move(label))
{
}

void GUI::CircleImageButton::Render(ImVec2 size)
{
	const std::shared_ptr<TextureAsset> texture = GetCurrentImage();

	if (texture)
	{
		ImDrawList *draw_list = ImGui::GetWindowDrawList();

		if (ImGui::InvisibleButton(m_label.c_str(), size))
		{
			m_isSelected = !m_isSelected;
		}

		const auto center = ImVec2((ImGui::GetItemRectMin().x + ImGui::GetItemRectMax().x) * 0.5f,
		                           (ImGui::GetItemRectMin().y + ImGui::GetItemRectMax().y) * 0.5f);
		const float diameter = std::min(ImGui::GetItemRectSize().y, ImGui::GetItemRectSize().x);
		const float radius = 0.5f * diameter;

		if (m_isSelected)
		{
			draw_list->AddCircleFilled(
				center,
				radius,
				m_selectColor,
				255.f);
		}

		draw_list->AddImage(
			texture->texture,
			ImVec2(center.x - radius * 0.5f,
			       center.y - radius * 0.5f),
			ImVec2(center.x + radius * 0.5f,
			       center.y + radius * 0.5f));
	}
	else
	{
		if (ImGui::Button(m_label.c_str()))
		{
			m_isSelected = !m_isSelected;
		}
	}
}

std::shared_ptr<GUI::TextureAsset> GUI::CircleImageButton::GetCurrentImage() const
{
	if (m_isSelected && m_onImage)
		return m_onImage;
	return m_offImage;
}
