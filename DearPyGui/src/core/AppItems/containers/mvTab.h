#pragma once

#include "mvTypeBases.h"

namespace Marvel {

	MV_REGISTER_WIDGET(mvTab, MV_ITEM_DESC_CONTAINER, StorageValueTypes::Bool, 1);
	class mvTab : public mvBoolPtrBase
	{

	public:

		static void InsertParser(std::map<std::string, mvPythonParser>* parsers);

		MV_APPLY_WIDGET_REGISTRATION(mvAppItemType::mvTab, add_tab)

		MV_CREATE_CONSTANT(mvThemeCol_Tab_Text, ImGuiCol_Text, 0L);
		MV_CREATE_CONSTANT(mvThemeCol_Tab_Border, ImGuiCol_Border, 0L);
		MV_CREATE_CONSTANT(mvThemeCol_Tab_Bg, ImGuiCol_Tab, 0L);
		MV_CREATE_CONSTANT(mvThemeCol_Tab_BgHovered, ImGuiCol_TabHovered, 0L);
		MV_CREATE_CONSTANT(mvThemeCol_Tab_BgActive, ImGuiCol_TabActive, 0L);
		MV_CREATE_CONSTANT(mvThemeCol_Tab_PopupBg, ImGuiCol_PopupBg	, 0L);

		MV_CREATE_CONSTANT(mvThemeStyle_Tab_Rounding, ImGuiStyleVar_TabRounding, 0L);
		MV_CREATE_CONSTANT(mvThemeStyle_Tab_PaddingX, ImGuiStyleVar_FramePadding, 0L);
		MV_CREATE_CONSTANT(mvThemeStyle_Tab_PaddingY, ImGuiStyleVar_FramePadding, 1L);
		MV_CREATE_CONSTANT(mvThemeStyle_Tab_InnerSpacingX, ImGuiStyleVar_ItemInnerSpacing, 0L);
		MV_CREATE_CONSTANT(mvThemeStyle_Tab_InnerSpacingY, ImGuiStyleVar_ItemInnerSpacing, 1L);

		MV_START_EXTRA_COMMANDS
		MV_END_EXTRA_COMMANDS

		MV_START_GENERAL_CONSTANTS
		MV_END_GENERAL_CONSTANTS

		MV_START_COLOR_CONSTANTS
			MV_CREATE_CONSTANT_PAIR(mvThemeCol_Tab_Text, mvImGuiCol_Text),
			MV_CREATE_CONSTANT_PAIR(mvThemeCol_Tab_Bg, mvImGuiCol_Tab),
			MV_CREATE_CONSTANT_PAIR(mvThemeCol_Tab_BgHovered, mvImGuiCol_TabHovered),
			MV_CREATE_CONSTANT_PAIR(mvThemeCol_Tab_BgActive, mvImGuiCol_TabActive),
			MV_CREATE_CONSTANT_PAIR(mvThemeCol_Tab_Border, mvImGuiCol_Border),
			MV_CREATE_CONSTANT_PAIR(mvThemeCol_Tab_PopupBg, mvImGuiCol_PopupBg),
		MV_END_COLOR_CONSTANTS

		MV_START_STYLE_CONSTANTS
			MV_ADD_CONSTANT_F(mvThemeStyle_Tab_Rounding, 0, 12),
			MV_ADD_CONSTANT_F(mvThemeStyle_Tab_PaddingX, 4, 20),
			MV_ADD_CONSTANT_F(mvThemeStyle_Tab_PaddingY, 3, 20),
			MV_ADD_CONSTANT_F(mvThemeStyle_Tab_InnerSpacingX, 4, 20),
			MV_ADD_CONSTANT_F(mvThemeStyle_Tab_InnerSpacingY, 4, 20),
		MV_END_STYLE_CONSTANTS

	public:

		mvTab(const std::string& name);

		void draw(ImDrawList* drawlist, float x, float y) override;
		void addFlag   (ImGuiTabItemFlags flag);
		void removeFlag(ImGuiTabItemFlags flag);

		void handleSpecificKeywordArgs(PyObject* dict) override;
		void getSpecificConfiguration(PyObject* dict) override;

	private:

		bool              m_closable = false;
		ImGuiTabItemFlags m_flags = ImGuiTabItemFlags_None;

	};

}