#include "texture_master.hpp"

#include "rng.hpp"

int texture_master::add_texture(std::string path, bool locked) {

    if (does_texture_exist(path) != -1) {
        return does_texture_exist(path);
    }

    int id = random_num.get_random_int();

    texture_archive* tex = new texture_archive;
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

    textures[id] = tex;



    return id;
}

void SafeUnloadTexture(Texture2D *tex) {
    if (tex && tex->id != 0) {
        UnloadTexture(*tex);
        tex->id = 0; // make sure it stays marked invalid
    }
}

Texture2D* texture_master::grab_texture_pointer(int id) {

    texture_archive* tex = textures[id];

    if (tex != nullptr) {

        if (tex->texture.id == 1 || tex->texture.id == 2 || tex->texture.id == 4) {
            std::cout << "ssssssssssssssssssssssssssssssssssss\n";
        }

        if (!tex->loaded || tex->texture.id == 0) {
            tex->needs_reload = true;
        }
        else {
            return &tex->texture;
        }
    }
    return &default_texture_T;

}


void texture_master::update() {
    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);

    // Pass 1: mark expired textures
    for (auto& [id, tex] : textures) {
        if (tex == nullptr) continue; // <- check here
        if (!tex->locked && tex->loaded && tex->expiration < current_time) {
            tex->loaded = false;
            tex->needs_reload = true; // defer actual unload
        }
    }

    // Pass 2: actually unload
    for (auto& [id, tex] : textures) {
        if (tex == nullptr) continue; // <- and here
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
    int count = 0;

    for (auto& [id, tex] : textures) {
        if (tex == nullptr) continue;
        SafeUnloadTexture(&tex->texture);
        count++;
    }

    std::cout << "(TEXTURE_MANAGER): Unloaded " << count << " textures\n";
}

void texture_master::print_all_ids() {
    for (auto& [id, tex] : textures) {
        if (tex == nullptr) continue;
        std::cout << "texture id: " << tex->texture.id << std::endl;
    }
}


int texture_master::does_texture_exist(const std::string &path) {
    for (auto& [id, tex] : textures) {
        if (tex != nullptr && tex->origin == path) {
            return id;
        }
    }
    return -1;
}
