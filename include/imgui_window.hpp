#include "imgui.h"
#include "rlImGui.h"
#include "item_master.hpp"

class imgui_win {
    private:
        bool items_menu_active = true;

    public:
        void items_menu();
};

extern imgui_win imgui_master;