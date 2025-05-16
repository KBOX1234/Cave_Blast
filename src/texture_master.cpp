#include "texture_master.hpp"

#include "rng.hpp"

int texture_master::add_texture(std::string path){

    const int id = random_num.get_random_int();
    texture_archive texture;
    texture.id = id;
    texture.texture = LoadTexture(path.c_str());
    texture.origin = path;
    texture.loaded = true;

    const auto now = std::chrono::system_clock::now();
    const auto future_time = now + std::chrono::minutes(1);

    texture.expiration = std::chrono::system_clock::to_time_t(future_time);

    textures.push_back(texture);

    return id;
}

Texture2D* texture_master::grab_texture_pointer(int id){
    for (int i = 0; i < textures.size(); i++) {
        if(textures[i].id == id){

            if(textures[i].loaded == false){
                textures[i].texture = LoadTexture(textures[i].origin.c_str());
                textures[i].loaded = true;
                auto now = std::chrono::system_clock::now();
                auto future_time = now + std::chrono::minutes(1);

                textures[i].expiration = std::chrono::system_clock::to_time_t(future_time);

            }

            return &textures[i].texture;
        }
    }

    return nullptr;
}

void texture_master::update(){
    for(int i = 0; i < textures.size(); i++){

        auto now = std::chrono::system_clock::now();
        std::time_t current_time = std::chrono::system_clock::to_time_t(now);

        if(textures[i].expiration < current_time){
            textures[i].loaded = false;
            UnloadTexture(textures[i].texture);
        }

    }
}