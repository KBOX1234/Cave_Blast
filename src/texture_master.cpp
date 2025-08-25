#include "texture_master.hpp"

#include "rng.hpp"

int texture_master::add_texture(std::string path, bool locked) {
    int id = random_num.get_random_int();

    auto tex = std::make_unique<texture_archive>();
    tex->id = id;
    tex->texture = LoadTexture(path.c_str());
    tex->origin = path;
    tex->loaded = true;
    tex->locked = locked;

    //std::cout << "(TEXTURE_MANAGER): Loaded texture: " << tex->texture.id << std::endl;

    if (tex->texture.id == 0) {
        std::cout << "(TEXTURE_MANAGER): Failed to load texture: " << path << std::endl;
        return default_texture();
    }

    auto now = std::chrono::system_clock::now();
    tex->expiration = std::chrono::system_clock::to_time_t(now + std::chrono::minutes(1));

    textures.push_back(std::move(tex));



    return id;
}

void SafeUnloadTexture(Texture2D *tex) {
    if (tex && tex->id != 0) {
        UnloadTexture(*tex);
        tex->id = 0; // make sure it stays marked invalid
    }
}

Texture2D* texture_master::grab_texture_pointer(int id) {
    for (auto& tex : textures) {
        if (tex->id == id) {

            if (tex->texture.id == 1 || tex->texture.id == 2 || tex->texture.id == 4) {
                std::cout << "ssssssssssssssssssssssssssssssssssss\n";
                break;
            }

            if (!tex->loaded || tex->texture.id == 0) {
                tex->needs_reload = true;
            }
            else {
                return &tex->texture;
            }
        }
    }
    return &default_texture_T;

}


void texture_master::update() {
    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);

    for (auto& tex : textures) {
        if (!tex->locked && tex->loaded && tex->expiration < current_time) {
            tex->loaded = false;
            tex->needs_reload = true; // defer actual unload
        }
    }

    // actually unload
    for (auto& tex : textures) {
        if (!tex->loaded && tex->texture.id != 0) {
            SafeUnloadTexture(&tex->texture);
        }
    }
}



int texture_master::default_texture() {
    return default_texture_id;
}

int texture_master::set_default_texture(std::string path) {
    default_texture_id = add_texture(path, true);

    default_texture_T = LoadTexture(path.c_str());

    std::cout << "(TEXTURE_MANAGER): Set default texture: " + path + "\n";

    return default_texture_id;
}

void texture_master::clean_up() {
    for (int i = 0; i < textures.size(); i++) {
        if (textures[i] == nullptr) continue;
        SafeUnloadTexture(&textures[i]->texture);
    }
}

void texture_master::print_all_ids() {
    for (int i = 0; i < textures.size(); i++) {
        std::cout << "texture id: " << textures[i]->texture.id << std::endl;
    }
}

