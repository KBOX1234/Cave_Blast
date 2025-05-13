#include "texture_master.hpp"

void texture_master::add_texture(Texture2D img, int id){
    texture_archive texture;
    texture.id = id;
    texture.texture = img;

    textures.push_back(texture);
}

Texture2D* texture_master::grab_texture_pointer(int id){
    for (int i = 0; i < textures.size(); i++) {
        if(textures[i].id == id) return &textures[i].texture;
    }

    return nullptr;
}