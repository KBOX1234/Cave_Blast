#pragma once
#include "json.hpp"
#include "raylib.h"
#include "player.hpp"

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
private:

    void update_movement();

    public:

    void update();
};

extern input input_manager;