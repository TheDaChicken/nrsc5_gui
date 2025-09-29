//
// Created by TheDaChicken on 9/27/2025.
//

#ifndef TEXTRENDER_H
#define TEXTRENDER_H

#include <imgui.h>
#include <string_view>

struct TextBlock
{
	ImFont *font;
	float font_size;
	std::string_view text;
};

class TextRender
{
	public:
		TextRender(ImVec2 size);
		~TextRender();

		void DrawBlock(const TextBlock &block);

	private:
		ImVec2 pos;
};

#endif //TEXTRENDER_H
