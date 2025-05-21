#pragma once
#include "imgui.h"
#include "rlImGui.h"
#include "item_master.hpp"
#include "player.hpp"

class imgui_win {
    friend class player_master;
    private:
        bool items_menu_active = true;

    public:
        void items_menu();

        void player_data_menu();
};

extern imgui_win imgui_master;