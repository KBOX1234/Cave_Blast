#pragma once
#include "json.hpp"
#include "raylib.h"
#include "world.hpp"
#include "player.hpp"
#include "imgui_window.hpp"

struct key_bind {
    int org_key;

    int custom_key;
};

int kb(key_bind kbind);

class keybind_master {

    std::vector<key_bind> key_binds;

    public:

    int load_key_bind(const std::string& path);

    int save_key_bind(const std::string& path);

    void new_keybind(int org_key, int custom_key);

    bool is_key_pressed(key_bind kbind);

    bool is_key_down(key_bind kbind);


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