#include "mvMenuBar.h"
#include "mvApp.h"
#include "mvLog.h"
#include "mvItemRegistry.h"
#include "mvImGuiThemeScope.h"
#include "containers/mvWindowAppItem.h"
#include "containers/mvChild.h"
#include "mvFontScope.h"
#include "mvPythonExceptions.h"

namespace Marvel {

	void mvMenuBar::InsertParser(std::map<std::string, mvPythonParser>* parsers)
	{

		mvPythonParser parser(mvPyDataType::String, "Undocumented function", { "Containers", "Widgets" });
		mvAppItem::AddCommonArgs(parser, (CommonParserArgs)(
			MV_PARSER_ARG_ID |
			MV_PARSER_ARG_INDENT |
			MV_PARSER_ARG_PARENT |
			MV_PARSER_ARG_SHOW)
		);
		parser.finalize();

		parsers->insert({ s_command, parser });
	}

	mvMenuBar::mvMenuBar(const std::string& name)
			: mvBoolPtrBase(name)
		{

			// TODO use code below to set item height when font and scale systems are done
			//float FontSize = ImGui::GetFontSize(); // = Base Font Size * Current Window Scale
			//ImGuiStyle currentStyle = ImGui::GetStyle(); // = Padding for the Top and Bottom
			//m_height = int(currentStyle.FramePadding.y * 2 + FontSize);
			m_height = 21;
		}

	void mvMenuBar::draw(ImDrawList* drawlist, float x, float y)
	{
		mvImGuiThemeScope scope(this);
		mvFontScope fscope(this);

		if (ImGui::BeginMenuBar())
		{
			//we do this so that the children dont get the theme
			scope.cleanup();

			for (auto& item : m_children[1])
			{
				if (!item->preDraw())
					continue;

				item->draw(drawlist, ImGui::GetCursorPosX(), ImGui::GetCursorPosY());

				item->postDraw();
			}
			ImGui::EndMenuBar();
		}
	}

	bool mvMenuBar::isParentCompatible(mvAppItemType type)
	{
		if (type == mvAppItemType::mvWindowAppItem) return true;
		if (type == mvAppItemType::mvChild) return true;

		mvThrowPythonError(1000, "Menubar parent must be a window or child.");
		MV_ITEM_REGISTRY_ERROR("Menubar parent must be a window or child.");
		assert(false);
		return false;
	}

}