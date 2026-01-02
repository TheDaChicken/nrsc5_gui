//
// Created by TheDaChicken on 7/30/2025.
//

#include "DockStationPanel.h"

#include <imgui.h>

extern "C" {
#include <nrsc5.h>
}

#include "gui/Util.h"
#include "gui/managers/ThemeManager.h"
#include "gui/utils/TextRender.h"
#include "gui/widgets/DualIconButton.h"

constexpr float kStationLogoScale = 3.0f;
constexpr int MAX_LINES = 2;

DockStationPanel::DockStationPanel(const std::shared_ptr<UISession> &session)
	: session_(session)
{
}

void DockStationPanel::Render(RenderContext &context)
{
	// const auto state = session_->GetHybridState();
	//
	// RenderStationHeader(context.theme, state);
	//
	// ImGui::PushFont(context.theme.GetFont(FontType::Semibold),
	//                 context.theme.font_very_large_size);
	// ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, ImGui::GetFontSize() / 2);
	//
	// const float footer_height = ImGui::GetFrameHeightWithSpacing();
	//
	// if (ImGui::BeginChild(
	// 	"##RadioInfo",
	// 	ImVec2(0, ImGui::GetContentRegionAvail().y - footer_height),
	// 	ImGuiChildFlags_AlwaysUseWindowPadding))
	// {
	// 	RenderRadioInfo(context.theme, state);
	// }
	// ImGui::EndChild();
	//
	// // Create a border only on bottom and top
	// ImDrawList *draw_list = ImGui::GetWindowDrawList();
	// const ImVec2 min = ImGui::GetItemRectMin();
	// const ImVec2 max = ImGui::GetItemRectMax();
	// draw_list->AddRectFilled(
	// 	ImVec2(min.x, max.y - context.theme.separator_thickness),
	// 	ImVec2(max.x, max.y),
	// 	ImGui::GetColorU32(ImGuiCol_Border));
	// draw_list->AddRectFilled(
	// 	ImVec2(min.x, min.y - context.theme.separator_thickness),
	// 	ImVec2(max.x, min.y),
	// 	ImGui::GetColorU32(ImGuiCol_Border));
	//
	// if (ImGui::BeginChild(
	// 	"##Footer",
	// 	ImVec2(0, 0)))
	// {
	// 	ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetColorU32(ImGuiCol_Separator));
	//
	// 	RenderControls(context.theme);
	//
	// 	ImGui::PopStyleColor();
	// }
	// ImGui::EndChild();
	//
	// ImGui::PopStyleVar();
	// ImGui::PopFont();
}

// void DockStationPanel::RenderStationHeader(
// 	const Theme &theme,
// 	const HybridState &state)
// {
// 	ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, ImGui::GetStyle().ItemSpacing.x * 0.5f);
// 	ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, ImGui::GetStyle().ItemSpacing.y * 0.5f);
//
// 	NRSC5::Program program;
//
// 	// const auto program_iter = state.data.programs.find(state.ui.program_id);
// 	// if (program_iter != state.data.programs.end())
// 	// 	program = program_iter->second;
//
// 	RenderStationImage(theme, state);
// 	ImGui::SameLine();
// 	RenderStationDetails(theme, state, program);
// 	ImGui::SameLine();
//
// 	ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_medium_size);
// 	ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, ImGui::GetStyle().ItemSpacing.x * 0.7f);
//
// 	const auto &texture = theme.GetIcon(IconType::HDLogo)->UpdateSize(
// 		ImGui::GetFontSize());
// 	// Render HD on right side
// 	ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
// 		std::max(0.0f,
// 		         ImGui::GetContentRegionAvail().x -
// 		         static_cast<float>(texture->GetWidth())));
// 	RenderProgramList(theme, state);
//
// 	ImGui::PopStyleVar();
// 	ImGui::PopFont();
//
// 	ImGui::PopStyleVar(2);
// }
//
// void DockStationPanel::RenderRadioInfo(
// 	const Theme &theme,
// 	const HybridState &state)
// {
// 	RenderPrimaryImage(theme, state);
// 	ImGui::SameLine();
//
// 	// const auto program_iter = state.data.programs.find(state.ui.program_id);
// 	// if (program_iter != state.data.programs.end())
// 	// 	RenderID3(theme, program_iter->second.id3_);
// }
//
// void DockStationPanel::RenderImage(
// 	const std::shared_ptr<GUI::ITexture> &texture,
// 	const float height)
// {
// 	const ImVec2 size = {
// 		static_cast<float>(texture->GetWidth()) * height
// 		/ static_cast<float>(texture->GetHeight()),
// 		height
// 	};
//
// 	ImGui::Image(texture->GetPtr(), size);
// }
//
// void DockStationPanel::RenderStationImage(
// 	const Theme &theme,
// 	const HybridState &state)
// {
// 	ImGui::PushFont(
// 		theme.GetFont(FontType::Semibold),
// 		theme.font_large_size * kStationLogoScale);
//
// 	const std::shared_ptr<GUI::ITexture> station_image = FirstLoadedTexture(
// 		{state.ui.station_logo_},
// 		theme.GetImage(ImageType::ChannelDefault));
//
// 	RenderImage(station_image, ImGui::GetFontSize());
// 	ImGui::PopFont();
// }
//
// void DockStationPanel::RenderPrimaryImage(
// 	const Theme &theme,
// 	const HybridState &state)
// {
// 	const std::shared_ptr<GUI::ITexture> primary_image = FirstLoadedTexture(
// 		{state.ui.primary_logo_, state.ui.station_logo_},
// 		theme.GetImage(ImageType::PrimaryDefault));
//
// 	RenderImage(primary_image, 250);
// }
//
// void DockStationPanel::RenderStationDetails(
// 	const Theme &theme,
// 	const HybridState &state,
// 	const NRSC5::Program &program)
// {
// 	const std::string_view longest_str = NRSC5::Decoder::GetProgramTypeName(
// 		NRSC5_PROGRAM_TYPE_SPECIAL_READING_SERVICES);
// 	float header_height = 0;
//
// 	ImGui::PushFont(theme.GetFont(FontType::Bold), theme.font_large_size);
// 	header_height += ImGui::GetTextLineHeightWithSpacing();
// 	ImGui::PopFont();
//
// 	ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_medium_size);
// 	header_height += MAX_LINES * ImGui::GetTextLineHeightWithSpacing();
// 	ImGui::PopFont();
//
// 	const float header_width = ImGui::CalcTextSize(
// 		longest_str.data(),
// 		longest_str.data() + longest_str.size()).x;
//
// 	TextRender render;
//
// 	render.BeginRender({header_width, header_height});
//
// 	render.DrawBlock({
// 		theme.GetFont(FontType::Bold),
// 		theme.font_large_size,
// 		state.ui.frequency_text
// 	});
//
// 	if (!state.ui.formatted_name.empty())
// 	{
// 		render.DrawBlock({
// 			theme.GetFont(FontType::Semibold),
// 			theme.font_medium_size,
// 			state.ui.formatted_name,
// 		});
// 	}
//
// 	if (program.type != NRSC5_PROGRAM_TYPE_UNDEFINED)
// 	{
// 		const std::string_view program_type =
// 				NRSC5::Decoder::GetProgramTypeName(program.type);
//
// 		render.DrawBlock({
// 			theme.GetFont(FontType::Semibold),
// 			theme.font_medium_size,
// 			program_type,
// 		});
// 	}
//
// 	render.EndRender();
// }
//
// void DockStationPanel::RenderProgramList(
// 	const Theme &theme,
// 	const HybridState &state)
// {
// 	const auto &hd_logo = theme.GetIcon(IconType::HDLogo)->GetTexture();
// 	const ImVec2 hd_size(static_cast<float>(hd_logo->GetWidth()), static_cast<float>(hd_logo->GetHeight()));
//
// 	// Push smaller font just for text
// 	ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_small_size);
//
// 	ImGui::BeginGroup();
//
// 	ImGui::Image(hd_logo->GetPtr(), hd_size);
//
// 	// // Center the program numbers below the logo
// 	// const float text_width =
// 	// 		ImGui::CalcTextSize("1").x * state.data.programs.size() +
// 	// 		ImGui::GetStyle().ItemSpacing.x * (state.data.programs.size() - 1);
//
// 	//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + Center(hd_size.x, text_width));
//
// 	//RenderProgramNumbers(state.data.programs, state.ui.program_id);
//
// 	ImGui::EndGroup();
//
// 	ImGui::PopFont();
// }
//
// void DockStationPanel::RenderProgramNumbers(
// 	const std::map<unsigned int, NRSC5::Program> &programs,
// 	const unsigned int currId)
// {
// 	if (programs.size() <= 1)
// 	{
// 		ImGui::TextUnformatted(""); // Placeholder for alignment
// 		return;
// 	}
//
// 	ImGui::BeginGroup();
//
// 	const auto program_frame = programs.begin();
//
// 	for (const auto &[id, program] : programs)
// 	{
// 		if (id != program_frame->first)
// 			ImGui::SameLine();
//
// 		if (id == currId)
// 			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 165, 0, 255));
//
// 		std::string number = fmt::format(
// 			"{}",
// 			NRSC5::FriendlyProgramId(id)
// 		);
//
// 		ImGui::TextUnformatted(number.data(), number.data() + number.size());
//
// 		if (id == currId)
// 			ImGui::PopStyleColor();
// 	}
//
// 	ImGui::EndGroup();
// }
//
// void DockStationPanel::RenderID3(const Theme &theme, const NRSC5::ID3 &id3)
// {
// 	ImGui::BeginGroup();
// 	ImGui::PushTextWrapPos(0.0f);
// 	{
// 		ImGui::PushFont(theme.GetFont(FontType::Bold), theme.font_large_size);
// 		ImGui::TextUnformatted(id3.title.c_str());
// 		ImGui::PopFont();
//
// 		ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_medium_size);
// 		ImGui::TextUnformatted(id3.artist.c_str());
// 		ImGui::TextUnformatted(id3.album.c_str());
// 		ImGui::TextUnformatted(id3.genre.c_str());
// 		ImGui::PopFont();
// 	}
// 	ImGui::PopTextWrapPos();
// 	ImGui::EndGroup();
// }
//
// void DockStationPanel::RenderControls(const Theme &theme) const
// {
// 	ImGui::BeginGroup();
//
// 	const GUI::DualIconButton channel_button{
// 		"ChannelButton",
// 		"Ch",
// 		{"ChannelLeft", theme.GetIcon(IconType::Minus)},
// 		{"ChannelRight", theme.GetIcon(IconType::Plus)}
// 	};
//
// 	switch (channel_button.Render())
// 	{
// 		case GUI::DualIconButton::Result::Left:
// 		{
// 			session_->PreviousChannel();
// 			break;
// 		}
// 		case GUI::DualIconButton::Result::Right:
// 		{
// 			session_->NextChannel();
// 			break;
// 		}
// 		default:
// 			break;
// 	}
//
// 	ImGui::EndGroup();
// }
