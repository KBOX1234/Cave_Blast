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

    auto now = std::chrono::system_clock::now();
    tex->expiration = std::chrono::system_clock::to_time_t(now + std::chrono::minutes(1));
    std::cout << "Added texture " << std::to_string(id) << " to the texture archive" << std::endl << "index is: " + std::to_string(textures.size());
    textures.push_back(std::move(tex));

    return id;
}


Texture2D* texture_master::grab_texture_pointer(int id) {
    for (auto& tex : textures) {
        if (tex->id == id) {
            if (!tex->loaded) {
                tex->texture = LoadTexture(tex->origin.c_str());
                tex->loaded = true;
                tex->expiration = std::chrono::system_clock::to_time_t(
                    std::chrono::system_clock::now() + std::chrono::minutes(1));
            }

            return &tex->texture;
        }
    }
    return nullptr;
}


void texture_master::update() {
    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);

    for (auto& tex : textures) {
        if (!tex->locked && tex->loaded && tex->expiration < current_time) {
            UnloadTexture(tex->texture);
            tex->loaded = false;
        }
    }
}


int texture_master::default_texture() {
    return default_texture_id;
}

int texture_master::set_default_texture(std::string path) {
    default_texture_id = add_texture(path, true);

    return default_texture_id;
}
