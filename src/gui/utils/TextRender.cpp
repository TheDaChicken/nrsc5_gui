//
// Created by TheDaChicken on 9/27/2025.
//

#include "TextRender.h"

TextRender::TextRender()
= default;

TextRender::~TextRender()
{

}

void TextRender::BeginRender(const ImVec2 size)
{
	ImGui::BeginChild("##Text", size);
}

void TextRender::EndRender()
{
	ImGui::EndChild();
}

void TextRender::DrawBlock(const TextBlock &block)
{
	ImGui::PushFont(block.font, block.font_size);
	ImGui::TextUnformatted(block.text.data(),
	                       block.text.data() + block.text.size());
	ImGui::PopFont();
}
