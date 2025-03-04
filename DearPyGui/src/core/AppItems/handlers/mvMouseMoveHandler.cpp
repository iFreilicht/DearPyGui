#include "mvMouseMoveHandler.h"
#include "mvLog.h"
#include "mvItemRegistry.h"
#include "mvPythonExceptions.h"
#include "mvUtilities.h"

namespace Marvel {

	void mvMouseMoveHandler::InsertParser(std::map<std::string, mvPythonParser>* parsers)
	{

		mvPythonParser parser(mvPyDataType::String, "Undocumented function", { "Textures", "Widgets" });
		mvAppItem::AddCommonArgs(parser, (CommonParserArgs)(
			MV_PARSER_ARG_ID |
			MV_PARSER_ARG_CALLBACK |
			MV_PARSER_ARG_PARENT)
		);
		parser.finalize();

		parsers->insert({ s_command, parser });
	}

	mvMouseMoveHandler::mvMouseMoveHandler(const std::string& name)
		:
		mvAppItem(name)
	{

	}

	bool mvMouseMoveHandler::isParentCompatible(mvAppItemType type)
	{
		if (type == mvAppItemType::mvStagingContainer) return true;
		if (type == mvAppItemType::mvHandlerRegistry) return true;

		mvThrowPythonError(1000, "Drawing item parent must be a drawing.");
		MV_ITEM_REGISTRY_ERROR("Drawing item parent must be a drawing.");
		assert(false);
		return false;
	}

	void mvMouseMoveHandler::draw(ImDrawList* drawlist, float x, float y)
	{

		// update mouse
		// mouse move event
		ImVec2 mousepos = ImGui::GetMousePos();
		if (ImGui::IsMousePosValid(&mousepos))
		{
			if (m_oldPos.x != mousepos.x || m_oldPos.y != mousepos.y)
			{
				m_oldPos = mousepos;

				mvApp::GetApp()->getCallbackRegistry().submitCallback([=]()
					{
						mvApp::GetApp()->getCallbackRegistry().runCallback(getCallback(false), m_name, ToPyPair(mousepos.x, mousepos.y), nullptr);
					});
			}
		}
	}
}