#pragma once
#include <vector>
#include <iostream>
#include <chrono>
#include <memory>
#include <unordered_map>
#include "raylib.h"

struct texture_archive{
    Texture2D texture;
    std::string origin;
    bool loaded;
    bool locked;
    bool needs_reload;

    std::time_t expiration;
};

class texture_master{
    private:
        //std::vector<std::unique_ptr<texture_archive>> textures;
        std::unordered_map<int, texture_archive*> textures;

        int default_texture_id;

        Texture2D default_texture_T;

        //if texture does not exist then return -1 otherwise it returns the texture id
        int does_texture_exist(const std::string& path);

    public:


        int default_texture();

        int add_texture(std::string path, bool locked = true);

        Texture2D* grab_texture_pointer(int id);

        int set_default_texture(std::string path);

        void update();

        void clean_up();

        void print_all_ids();


};

extern texture_master texture_manager;
