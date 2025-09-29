//
// Created by TheDaChicken on 8/3/2025.
//

#include "DockFavoritesPanel.h"

#include "gui/managers/ThemeManager.h"
#include "gui/widgets/ListView.h"

DockFavoritesPanel::DockFavoritesPanel(
	const std::shared_ptr<HybridExternal> &external,
	FavoriteList &favorite_model)
	: favorite_model_(favorite_model), external_service_(external)
{
	favorite_model_.on_update = [this]
	{
		m_items.resize(favorite_model_.GetChannelCount());

		for (int i = 0; i < favorite_model_.GetChannelCount(); ++i)
		{
			const Station &fav_station = favorite_model_.GetChannel(i);
			FavoriteItem &item = m_items[i];

			item.station = &fav_station;
			item.name = item.station->GetShort();

			if (external_service_)
			{
				const MetadataQuery query{
					item.station->identity.country_code,
					item.station->identity.name,
					MimeQuery::StationLogo,
					item.station->program_id
				};

				item.icon = external_service_->FetchImageAsync(query);
			}
		}

		// Keep selection valid
		if (m_selectedItem >= favorite_model_.GetChannelCount())
			m_selectedItem = 0;
	};
}

bool DockFavoritesPanel::Render(const Theme &theme)
{
	ImGui::BeginChild("FavoritesList",
	                  {0, 0});

	ImGui::PushFont(theme.GetFont(FontType::Semibold), theme.font_medium_size);
	ImGui::PushStyleVarY(ImGuiStyleVar_ItemInnerSpacing, 0);
	ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0);

	RenderHeader(theme);
	const bool changed = RenderItems(theme);

	ImGui::PopStyleVar(2);
	ImGui::PopFont();

	ImGui::EndChild();
	return changed;
}

void DockFavoritesPanel::RenderHeader(const Theme &theme)
{
	constexpr std::string_view favorite_text = "Favorites";
	constexpr std::string_view edit_text = "Edit";

	const ImVec2 sub_size = ImGui::CalcTextSize(
		edit_text.data(),
		edit_text.data() + edit_text.size());
	const ImVec2 main_size = ImGui::CalcTextSize(
		favorite_text.data(),
		favorite_text.data() + favorite_text.size());
	const float fav_height = ImGui::GetFontSize() + theme.separator_thickness
			+ ImGui::GetStyle().ItemInnerSpacing.y;

	ImGui::Dummy({ImGui::GetContentRegionAvail().x, fav_height});

	ImVec2 pos = ImGui::GetItemRectMin();
	ImVec2 max_pos = ImGui::GetItemRectMax();
	pos.x += ImGui::GetStyle().FramePadding.x;
	max_pos.y -= theme.separator_thickness;
	max_pos.x -= ImGui::GetStyle().FramePadding.x;
	ImDrawList *draw_list = ImGui::GetWindowDrawList();

	const float left_x = pos.x;
	const float right_x = std::max(
		max_pos.x - sub_size.x,
		left_x + main_size.x + ImGui::GetStyle().ItemInnerSpacing.y);

	draw_list->AddText(
		ImVec2(pos.x, pos.y + ImGui::GetStyle().ItemInnerSpacing.y),
		ImGui::GetColorU32(ImGuiCol_Text),
		favorite_text.data(),
		favorite_text.data() + favorite_text.size());

	draw_list->AddText(
		ImVec2(right_x,
		       pos.y + ImGui::GetStyle().ItemInnerSpacing.y),
		ImGui::GetColorU32(ImGuiCol_Text),
		edit_text.data(),
		edit_text.data() + edit_text.size());

	draw_list->AddRectFilled(
		ImVec2(pos.x, max_pos.y),
		ImVec2(max_pos.x, max_pos.y + theme.separator_thickness),
		ImGui::GetColorU32(ImGuiCol_Separator));
}

bool DockFavoritesPanel::RenderItems(const Theme &theme)
{
	bool changed = false;

	for (int i = 0; i < m_items.size(); ++i)
	{
		const FavoriteItem &favorite_item = m_items[i];

		const std::shared_ptr<GPU::Texture> &icon =
				favorite_item.icon.IsLoaded()
					? favorite_item.icon.get()
					: theme.GetImage(ImageType::ChannelDefault);

		ListView::Item item;
		item.name = favorite_item.name;
		item.image = icon.get();

		if (ListView::RenderItem(item, i == m_selectedItem))
		{
			changed = true;
			m_selectedItem = i;
		}
	}

	return changed;
}
