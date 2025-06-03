#pragma once
#include "json.hpp"
#include "raylib.h"

struct key_bind {
    int org_key;

    int custom_key;
};

int kb(key_bind kbind);

class keybind_master {

    public:
        int inventory_key = KEY_E;
        int move_cursor_lefty_key = KEY_LEFT;
        int move_cursor_righty_key = KEY_RIGHT;
        int move_cursor_upy_key = KEY_UP;
        int move_cursor_downy_key = KEY_DOWN;
        int move_lefty_key = KEY_A;
        int move_righty_key = KEY_D;
        int move_upy_key = KEY_W;
        int move_downy_key = KEY_S;
        int break_block_key = KEY_Q;
        int place_block_key = KEY_SPACE;
        int move_inventory_slot_left = KEY_J;
        int move_inventory_slot_right = KEY_L;
        int move_inventory_slot_up = KEY_I;
        int move_inventory_slot_down = KEY_K;


};

extern keybind_master keybind_manager;

class input {
    friend class imgui_win;

    private:

        bool is_inp_update;

        bool is_block_update;

        void update_movement();

        void mine_controles();

    public:

        bool is_there_input_update();

        Vector2 cursor = {0, 0};

        Vector2 cursor_offset = {0, 0};

        void update();
};

extern input input_manager;