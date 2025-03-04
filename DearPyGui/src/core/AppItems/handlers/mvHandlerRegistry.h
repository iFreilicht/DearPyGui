#pragma once

#include "mvAppItem.h"
#include "mvItemRegistry.h"
#include "mvModule_Core.h"
#include "cpp.hint"

namespace Marvel {

    MV_REGISTER_WIDGET(mvHandlerRegistry, MV_ITEM_DESC_ROOT | MV_ITEM_DESC_CONTAINER, StorageValueTypes::None, 1);
    class mvHandlerRegistry : public mvAppItem
    {

    public:

        static void InsertParser(std::map<std::string, mvPythonParser>* parsers);

        MV_APPLY_WIDGET_REGISTRATION(mvAppItemType::mvHandlerRegistry, add_handler_registry)

        MV_START_EXTRA_COMMANDS
        MV_END_EXTRA_COMMANDS

        MV_START_GENERAL_CONSTANTS
        MV_END_GENERAL_CONSTANTS

        MV_START_COLOR_CONSTANTS
        MV_END_COLOR_CONSTANTS

        MV_START_STYLE_CONSTANTS
        MV_END_STYLE_CONSTANTS

    public:

        mvHandlerRegistry(const std::string& name);

        void draw(ImDrawList* drawlist, float x, float y) override;

        bool canChildBeAdded(mvAppItemType type) override;

    };

}
