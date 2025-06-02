#pragma once
#include "imgui.h"
#include "imgui.h"
#include "rlImGui.h"
#include "item_master.hpp"
#include "block_master.hpp"
#include "texture_master.hpp"
#include "player.hpp"
#include "world.hpp"
#include "input.hpp"

class imgui_win {
    friend class player_master;
    private:
        bool items_menu_active = true;

    public:
        void items_menu();

        void player_data_menu();
};

extern imgui_win imgui_master;