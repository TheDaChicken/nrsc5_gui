//
// Created by TheDaChicken on 9/15/2025.
//

#include "DockTunePanel.h"

#include "gui/Util.h"

static int64_t numDigits(const int64_t n)
{
	if (n == 0)
		return 0;

	return static_cast<int>(std::floor(std::log10(std::abs(n)))) + 1;
}

static int64_t removeDigits(const int64_t n, const int64_t digits)
{
	if (digits <= 0)
		return n;

	return n / static_cast<int64_t>(pow(10, digits));
}

DockTunePanel::DockTunePanel(HybridInput &input)
	: state_(), input_(input)
{
	UpdateState(Band::FM);
}

bool DockTunePanel::Render(const Theme &theme)
{
	ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_large_size);

	ImGui::BeginGroup();

	RenderBands();

	ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_very_large_size);
	RenderFreq();
	ImGui::PopFont();

	RenderButtons();
	const bool tuned = RenderTuneButton();

	ImGui::EndGroup();

	ImGui::PopFont();
	return tuned;
}

void DockTunePanel::UpdateState(const Band::Type selected)
{
	const Band::Info info = Band::GetInfo(selected);

	state_.decimal_places = info.decimal_places;
	state_.scale = static_cast<uint32_t>(powf(10, static_cast<float>(info.decimal_places)));
	state_.freq_scale_den = info.scale;

	state_.freq_min = static_cast<int64_t>(info.minFrequency * (state_.scale / state_.freq_scale_den));
	state_.freq_max = static_cast<int64_t>(info.maxFrequency * (state_.scale / state_.freq_scale_den));

	selected_band_ = selected;
}

void DockTunePanel::RenderBand(const std::string &label, const Band::Type band)
{
	if (band == selected_band_)
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]);
	else
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

	if (ImGui::Button(label.c_str(), ImVec2(-FLT_MIN, 0)))
		UpdateState(band);

	ImGui::PopStyleColor();
}

void DockTunePanel::RenderBands()
{
	ImGui::BeginTable("#TunePanel",
	                  2,
	                  ImGuiTableFlags_SizingStretchSame |
	                  ImGuiTableFlags_NoPadOuterX);

	{
		ImGui::TableSetupColumn("Band");

		ImGui::TableNextColumn();
		RenderBand("FM", Band::FM);
		ImGui::TableNextColumn();
		RenderBand("AM", Band::AM);

		ImGui::EndTable();
	}
}

void DockTunePanel::RenderButtons()
{
	const int64_t used_digit = numDigits(state_.selected_freq);
	const int64_t min_digit = removeDigits(
		state_.freq_min,
		numDigits(state_.freq_min) - used_digit - 1);
	const int64_t max_digit = removeDigits(
		state_.freq_max,
		numDigits(state_.freq_max) - used_digit - 1);

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y);

	ImGui::BeginTable("#TuneButtons",
	                  5,
	                  ImGuiTableFlags_SizingStretchSame |
	                  ImGuiTableFlags_NoPadOuterX);

	for (int i = 0; i < 10; i++)
	{
		ImGui::TableNextColumn();

		const auto freq_str = fmt::format("{}", i);
		const int64_t prefix = state_.selected_freq * 10 + i;

		bool enabled_button;

		if (min_digit > max_digit)
			enabled_button = prefix >= min_digit || prefix <= max_digit && prefix != 0;
		else
			enabled_button = prefix >= min_digit && prefix <= max_digit;

		if (!enabled_button)
			ImGui::BeginDisabled();

		if (ImGui::Button(freq_str.c_str(), ImVec2(-FLT_MIN, 0)))
			state_.selected_freq = prefix;

		if (!enabled_button)
			ImGui::EndDisabled();
	}
	ImGui::EndTable();

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y);
}

void DockTunePanel::RenderFreq() const
{
	const ImVec2 size = {ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 5.0f};

	ImGui::Dummy(size);

	std::string frequency;
	if (state_.selected_freq != 0)
	{
		frequency = fmt::format(
			"{:.{}f}",
			static_cast<double>(state_.selected_freq) / state_.scale,
			state_.decimal_places);
	}
	else
	{
		frequency = "Select a frequency";
	}

	const ImVec2 pos = ImGui::GetItemRectMin();

	// Center text
	const ImVec2 text_size = ImGui::CalcTextSize(
		frequency.data(),
		frequency.data() + frequency.size()
	);
	const ImVec2 center = {
		pos.x + Center(size.x, text_size.x),
		pos.y + Center(size.y, text_size.y)
	};

	ImDrawList *draw_list = ImGui::GetWindowDrawList();
	draw_list->AddText(
		center,
		ImGui::GetColorU32(ImGuiCol_Text),
		frequency.data(),
		frequency.data() + frequency.size()
	);
}

bool DockTunePanel::RenderTuneButton()
{
	bool selected_tuned = false;
	const bool finished = state_.selected_freq >= state_.freq_min && state_.selected_freq <= state_.freq_max;

	if (!finished)
		ImGui::BeginDisabled();

	if (ImGui::Button("Tune", ImVec2(-FLT_MIN, 0)))
	{
		Station station;
		station.freq = static_cast<uint32_t>(
					static_cast<double>(state_.selected_freq) * state_.freq_scale_den)
				/ state_.scale;
		station.mode = selected_band_;
		station.program_id = 0; // default to HD1

		input_.SetChannel(station);

		state_.selected_freq = 0;
		selected_tuned = true;
	}

	if (!finished)
		ImGui::EndDisabled();
	return selected_tuned;
}
