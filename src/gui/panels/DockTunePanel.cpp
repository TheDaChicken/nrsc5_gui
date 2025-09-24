//
// Created by TheDaChicken on 9/15/2025.
//

#include "DockTunePanel.h"

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
	ImGui::BeginGroup();
	ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0);
	ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_large_size);

	const float total_height = ImGui::GetContentRegionAvail().y;
	constexpr float total_weight = 1 + 2 + 4 + 1;

	const float band_height = total_height * (1.0f / total_weight);
	const float freq_height = total_height * (2.0f / total_weight);
	const float buttons_height = total_height * (4.0f / total_weight);
	const float tune_height = total_height * (1.0f / total_weight);

	RenderBands(band_height);

	ImGui::BeginChild("Freq", ImVec2(0, freq_height));
	RenderFreq();
	ImGui::EndChild();

	RenderButtons(buttons_height);

	const bool tuned = RenderTuneButton(tune_height);

	ImGui::PopFont();
	ImGui::PopStyleVar();
	ImGui::EndGroup();
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

void DockTunePanel::RenderBand(const std::string &label, const Band::Type band, const float height)
{
	if (band == selected_band_)
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]);
	else
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

	if (ImGui::Button(label.c_str(), ImVec2(-FLT_MIN, height)))
		UpdateState(band);

	ImGui::PopStyleColor();
}

void DockTunePanel::RenderBands(const float height)
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

	const float row_height = GetRowHeight(height, 1);

	ImGui::BeginTable("#TunePanel",
	                  2,
	                  ImGuiTableFlags_SizingStretchSame |
	                  ImGuiTableFlags_NoPadOuterX);

	{
		ImGui::TableSetupColumn("Band");

		ImGui::TableNextColumn();
		RenderBand("FM", Band::FM, row_height);
		ImGui::TableNextColumn();
		RenderBand("AM", Band::AM, row_height);

		ImGui::EndTable();
	}
	ImGui::PopStyleVar();
}

void DockTunePanel::RenderButtons(const float height)
{
	const int64_t used_digit = numDigits(state_.selected_freq);
	const int64_t min_digit = removeDigits(
		state_.freq_min,
		numDigits(state_.freq_min) - used_digit - 1);
	const int64_t max_digit = removeDigits(
		state_.freq_max,
		numDigits(state_.freq_max) - used_digit - 1);

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y);

	const float total_height = height - ImGui::GetStyle().FramePadding.y * 2.0f;
	constexpr int rows = 10 / 5;
	const float rows_height = GetRowHeight(total_height, rows);

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

		if (ImGui::Button(freq_str.c_str(), ImVec2(-FLT_MIN, rows_height)))
			state_.selected_freq = prefix;

		if (!enabled_button)
			ImGui::EndDisabled();
	}
	ImGui::EndTable();

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y);
}

void DockTunePanel::RenderFreq() const
{
	ImGui::PushFont(nullptr, ImGui::GetFontSize() * 1.1f);

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

	// Center text
	const ImVec2 text_size = ImGui::CalcTextSize(
		frequency.data(),
		frequency.data() + frequency.size()
	);
	const float avail_x = ImGui::GetContentRegionAvail().x;
	const float avail_y = ImGui::GetContentRegionAvail().y;

	ImGui::SetCursorPos({
		ImGui::GetCursorPosX() + (avail_x - text_size.x) * 0.5f,
		ImGui::GetCursorPosY() + (avail_y - text_size.y) * 0.5f,
	});

	ImGui::TextUnformatted(
		frequency.data(),
		frequency.data() + frequency.size()
	);

	ImGui::PopFont();
}

bool DockTunePanel::RenderTuneButton(const float height)
{
	bool selected_tuned = false;
	const bool finished = state_.selected_freq >= state_.freq_min && state_.selected_freq <= state_.freq_max;

	if (!finished)
		ImGui::BeginDisabled();

	if (ImGui::Button("Tune", ImVec2(-FLT_MIN, height)))
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

float DockTunePanel::GetRowHeight(const float avail_height, const float rows)
{
	const float item_spacing = ImGui::GetStyle().ItemSpacing.y; // spacing between rows
	const float cell_spacing = ImGui::GetStyle().CellPadding.y * 2.0f;
	const float total_spacing = item_spacing * (rows - 1) + cell_spacing * rows;

	return std::max(avail_height - total_spacing, 0.0f) / rows;
}
