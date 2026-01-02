//
// Created by TheDaChicken on 8/3/2025.
//

#include "DockFavoritesPanel.h"

#include "gui/managers/ThemeManager.h"
#include "gui/widgets/ListView.h"

bool DockFavoritesPanel::Render(const RenderContext &context)
{
	ImGui::BeginChild("FavoritesList",
	                  {0, 0});

	ImGui::PushFont(
		context.theme.GetFont(FontType::Semibold),
		context.theme.font_medium_size);
	ImGui::PushStyleVarY(ImGuiStyleVar_ItemInnerSpacing, 0);
	ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0);

	RenderHeader(context.theme);
	const bool changed = RenderItems(context);

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

bool DockFavoritesPanel::RenderItems(const RenderContext &context) const
{
	bool changed = false;
	auto &items = context.app->fc->GetFavorites();
	int selected = context.app->fc->GetSelectedIndex();

	for (int i = 0; i < items.size(); ++i)
	{
		const FavoritesController::FavoriteItem &favorite_item = items[i];
		const std::shared_ptr<GUI::ITexture> &icon =
				favorite_item.icon.IsLoaded()
					? favorite_item.icon.Get()
					: context.theme.GetImage(ImageType::ChannelDefault);

		ListView::Item item;
		item.name = favorite_item.name;
		item.image = icon;

		if (ListView::RenderItem(item, i == selected))
		{
			changed = true;
			selected = i;
		}
	}

	context.app->fc->SetSelectedIndex(selected);
	return changed;
}
