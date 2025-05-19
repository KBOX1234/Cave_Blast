#include <vector>
#include <iostream>
#include <chrono>
#include <memory>
#include "raylib.h"

#pragma once

struct texture_archive{
    int id;
    Texture2D texture;
    std::string origin;
    bool loaded;
    bool locked;

    std::time_t expiration;
};

class texture_master{
    private:
    std::vector<std::unique_ptr<texture_archive>> textures;

        int default_texture_id;

        Texture2D default_texture_T;

    public:

        ~texture_master();

        int default_texture();

        int add_texture(std::string path, bool locked = false);

        Texture2D* grab_texture_pointer(int id);

        int set_default_texture(std::string path);

        void update();
};

extern texture_master texture_manager;
