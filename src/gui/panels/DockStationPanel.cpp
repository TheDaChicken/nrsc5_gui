//
// Created by TheDaChicken on 7/30/2025.
//

#include "DockStationPanel.h"

#include <imgui.h>

#include "gui/Util.h"
#include "gui/managers/ThemeManager.h"
#include "gui/utils/TextRender.h"
#include "gui/widgets/DualIconButton.h"

constexpr float kStationLogoScale = 2.5f;
constexpr int MAX_LINES = 2;

DockStationPanel::DockStationPanel(
	HybridInput &input,
	const std::weak_ptr<HybridSession> &session)
	: input_(input),
	  session_(session)
{
}

void DockStationPanel::Render(const Theme &theme)
{
	const auto session = session_.lock();

	frame_state_ = input_.Sessions().GetState();
	frame_program_ = session ? session->GetState() : ProgramState{};
	frame_programs_ = input_.Sessions().GetPrograms();

	RenderStationHeader(theme);

	ImGui::PushFont(theme.GetFont(FontType::Semibold),
	                theme.font_very_large_size);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, ImGui::GetFontSize() / 2);

	const float footer_height = ImGui::GetFrameHeightWithSpacing();

	if (ImGui::BeginChild(
		"##RadioInfo",
		ImVec2(0, -footer_height),
		ImGuiChildFlags_AlwaysUseWindowPadding))
	{
		RenderRadioInfo(theme);
	}
	ImGui::EndChild();

	// Create a border only on bottom and top
	ImDrawList *draw_list = ImGui::GetWindowDrawList();
	const ImVec2 min = ImGui::GetItemRectMin();
	const ImVec2 max = ImGui::GetItemRectMax();
	draw_list->AddRectFilled(
		ImVec2(min.x, max.y - theme.separator_thickness),
		ImVec2(max.x, max.y),
		ImGui::GetColorU32(ImGuiCol_Border));
	draw_list->AddRectFilled(
		ImVec2(min.x, min.y - theme.separator_thickness),
		ImVec2(max.x, min.y),
		ImGui::GetColorU32(ImGuiCol_Border));

	if (ImGui::BeginChild(
		"##Footer",
		ImVec2(0, 0)))
	{
		ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetColorU32(ImGuiCol_Separator));

		RenderControls(theme);

		ImGui::PopStyleColor();
	}
	ImGui::EndChild();

	ImGui::PopStyleVar();
	ImGui::PopFont();
}

void DockStationPanel::RenderStationHeader(const Theme &theme)
{
	ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, ImGui::GetStyle().ItemSpacing.x * 0.5f);
	ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, ImGui::GetStyle().ItemSpacing.y * 0.5f);

	RenderStationLogo(theme);
	ImGui::SameLine();
	RenderStationDetails(theme);
	ImGui::SameLine();

	ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_medium_size);
	ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, ImGui::GetStyle().ItemSpacing.x * 0.7f);
	{
		const auto &texture = theme.GetIcon(IconType::HDLogo)->UpdateSize(
			ImGui::GetFontSize());

		// Render HD on right side
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
			std::max(0.0f,
			         ImGui::GetContentRegionAvail().x -
			         static_cast<float>(texture.width)));
		RenderProgramList(theme);
	}
	ImGui::PopStyleVar();
	ImGui::PopFont();

	ImGui::PopStyleVar(2);
}

void DockStationPanel::RenderRadioInfo(const Theme &theme) const
{
	ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, ImGui::GetStyle().ItemSpacing.y * 2.0f);

	RenderPrimaryImage(theme);
	ImGui::SameLine();
	RenderID3(theme, frame_program_.id3);

	ImGui::PopStyleVar();
}

void DockStationPanel::RenderStationLogo(const Theme &theme) const
{
	ImGui::PushFont(
		theme.GetFont(FontType::Semibold),
		theme.font_medium_size * kStationLogoScale);
	const float station_logo_height = ImGui::GetFontSize();

	const std::shared_ptr<GPU::Texture> station_image = FirstLoadedTexture(
		{frame_program_.station_logo},
		theme.GetImage(ImageType::ChannelDefault));

	const ImVec2 size = {
		static_cast<float>(station_image->width) *
		station_logo_height / static_cast<float>(station_image->height),
		station_logo_height
	};

	ImGui::Image(reinterpret_cast<intptr_t>(station_image->ptr.get()), size);
	ImGui::PopFont();
}

void DockStationPanel::RenderStationDetails(const Theme &theme) const
{
	const char *longest_str;
	float header_height = 0;

	ImGui::PushFont(theme.GetFont(FontType::Bold), theme.font_large_size);
	header_height += ImGui::GetTextLineHeightWithSpacing();
	ImGui::PopFont();

	ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_medium_size);
	header_height += MAX_LINES * ImGui::GetTextLineHeightWithSpacing();
	ImGui::PopFont();

	nrsc5_program_type_name(
		NRSC5_PROGRAM_TYPE_SPECIAL_READING_SERVICES,
		&longest_str
	);

	const float header_width = ImGui::CalcTextSize(longest_str).x;

	TextRender render({header_width, header_height});

	render.DrawBlock({
		theme.GetFont(FontType::Bold),
		theme.font_large_size,
		frame_state_.frequency_text
	});

	if (!frame_program_.formatted_name.empty())
	{
		render.DrawBlock({
			theme.GetFont(FontType::Semibold),
			theme.font_medium_size,
			frame_program_.formatted_name,
		});
	}

	if (frame_program_.type != NRSC5_PROGRAM_TYPE_UNDEFINED)
	{
		const char *program_type_str;
		nrsc5_program_type_name(
			frame_program_.type,
			&program_type_str
		);

		render.DrawBlock({
			theme.GetFont(FontType::Semibold),
			theme.font_medium_size,
			program_type_str,
		});
	}
}

void DockStationPanel::RenderProgramList(const Theme &theme)
{
	const auto &hd_logo = theme.GetIcon(IconType::HDLogo)->GetTexture();
	const ImVec2 hd_size(static_cast<float>(hd_logo.width), static_cast<float>(hd_logo.height));

	// Push smaller font just for text
	ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_small_size);

	ImGui::BeginGroup();
	{
		ImGui::Image((intptr_t)hd_logo.ptr.get(), hd_size);

		// Center the program numbers below the logo
		const float text_width =
				ImGui::CalcTextSize("1").x * frame_programs_.size() +
				ImGui::GetStyle().ItemSpacing.x * (frame_programs_.size() - 1);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
			Center(hd_size.x, text_width));

		RenderProgramNumbers(frame_programs_);
	}
	ImGui::EndGroup();

	ImGui::PopFont();
}

void DockStationPanel::RenderProgramNumbers(
	const std::map<unsigned int, ProgramState> &programs) const
{
	if (programs.size() <= 1)
	{
		ImGui::TextUnformatted(""); // Placeholder for alignment
		return;
	}

	ImGui::BeginGroup();

	const auto program_frame = programs.begin();

	for (const auto &[id, program] : programs)
	{
		if (id != program_frame->first)
			ImGui::SameLine();

		if (id == frame_program_.id)
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 165, 0, 255));

		std::string number = fmt::format(
			"{}",
			NRSC5::FriendlyProgramId(id)
		);

		ImGui::TextUnformatted(number.data(), number.data() + number.size());

		if (id == frame_program_.id)
			ImGui::PopStyleColor();
	}

	ImGui::EndGroup();
}

void DockStationPanel::RenderPrimaryImage(const Theme &theme) const
{
	const std::shared_ptr<GPU::Texture> primary_image = FirstLoadedTexture(
		{frame_program_.primary_image, frame_program_.station_logo},
		theme.GetImage(ImageType::PrimaryDefault));

	ImGui::Image((intptr_t)primary_image->ptr.get(), {300, 300});
}

void DockStationPanel::RenderID3(const Theme &theme, const NRSC5::ID3 &id3)
{
	ImGui::BeginGroup();
	{
		ImGui::PushFont(theme.GetFont(FontType::Bold), theme.font_large_size);
		ImGui::TextWrapped("%s", id3.title.c_str());
		ImGui::PopFont();

		ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_medium_size);
		ImGui::TextWrapped("%s", id3.artist.c_str());
		ImGui::TextWrapped("%s", id3.album.c_str());
		ImGui::TextWrapped("%s", id3.genre.c_str());
		ImGui::PopFont();
	}
	ImGui::EndGroup();
}

void DockStationPanel::RenderControls(const Theme &theme) const
{
	// ImGui::SetCursorPosY(ImGui::GetCursorPosY() +
	// 	std::max(0.0f, (ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeight()) * 0.5f));
	ImGui::BeginGroup();

	const GUI::DualIconButton channel_button{
		"ChannelButton",
		"Ch",
		{"ChannelLeft", theme.GetIcon(IconType::Minus)},
		{"ChannelRight", theme.GetIcon(IconType::Plus)}
	};

	switch (channel_button.Render())
	{
		case GUI::DualIconButton::Result::Left:
		{
			if (const auto &session = session_.lock())
				input_.PreviousChannel(session);
			break;
		}
		case GUI::DualIconButton::Result::Right:
		{
			if (const auto &session = session_.lock())
				input_.NextChannel(session);
			break;
		}
		default:
			break;
	}

	ImGui::EndGroup();
}
