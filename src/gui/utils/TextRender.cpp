//
// Created by TheDaChicken on 9/27/2025.
//

#include "TextRender.h"

TextRender::TextRender(const ImVec2 size)
{
	ImGui::Dummy(size);

	const ImVec2 p0 = ImGui::GetItemRectMin();
	const ImVec2 p1 = ImGui::GetItemRectMax();

	ImGui::PushClipRect(p0, p1, true);

	pos = p0;
}

TextRender::~TextRender()
{
	ImGui::PopClipRect();
}

void TextRender::DrawBlock(const TextBlock &block)
{
	ImDrawList *draw_list = ImGui::GetWindowDrawList();

	ImGui::PushFont(block.font, block.font_size);

	const ImVec2 textSize = ImGui::CalcTextSize(
		block.text.data(),
		block.text.data() + block.text.size());

	draw_list->AddText(pos,
	                   ImGui::GetColorU32(ImGuiCol_Text),
	                   block.text.data(),
	                   block.text.data() + block.text.size());
	pos.y += textSize.y + ImGui::GetStyle().ItemSpacing.y;
	ImGui::PopFont();
}
