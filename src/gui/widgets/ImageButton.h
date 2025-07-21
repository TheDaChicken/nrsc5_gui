//
// Created by TheDaChicken on 7/13/2025.
//

#ifndef BUTTON_H
#define BUTTON_H
#include <memory>
#include <string>

#include "gui/TextureAsset.h"

namespace GUI
{
class CircleImageButton
{
	public:
		explicit CircleImageButton(std::string label);

		void SetOnImage(const std::shared_ptr<TextureAsset> &image)
		{
			m_onImage = image;
		}

		void SetOffImage(const std::shared_ptr<TextureAsset> &image)
		{
			m_offImage = image;
		}

		void SetSelectColor(const ImVec4 &color)
		{
			m_selectColor = color;
		}

		[[nodiscard]] std::string GetLabel() const
		{
			return m_label;
		}

		[[nodiscard]] ImVec2 GetSize() const
		{
			return m_offImage->size;
		}

		void Render(ImVec2 size);

	private:
		std::string m_label;
		std::shared_ptr<TextureAsset> m_onImage;
		std::shared_ptr<TextureAsset> m_offImage;
		std::shared_ptr<TextureAsset> GetCurrentImage() const;

		bool m_isSelected = false;
		ImColor m_selectColor = IM_COL32(140, 181, 255, 50);
};
}

#endif //BUTTON_H
