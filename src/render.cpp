#include "../include/render.hpp"

#define BLOCK_SIZE 32

void render::set_camera_target(Vector2 pos) {
    camera.target = pos;
}

void render::set_camera_zoom(float zoom) {
    camera.zoom = zoom;
}

void render::move_camera_x(float amount) {
    camera.target.x += amount;
}

void render::move_camera_y(float amount) {
    camera.target.y += amount;
}

void render::draw_chunk(Vector2 chnk_pos) {

    chnk_pos.x = round(chnk_pos.x);
    chnk_pos.y = round(chnk_pos.y);

    const block* render_chnk = world.chunk_buffer(chnk_pos);
    Vector2 pointer = {0, 0};

    for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++) {

        if (pointer.x >= CHUNK_SIZE) {
            pointer.x = 0;
            pointer.y++;
        }
        pointer.x++;

        if (render_chnk == nullptr) {
            std::cout << "somthing went wrong" << std::endl;
            break;
        }

        int texture_id = render_chnk[i].attr->texture_id;

        Texture2D* t = texture_manager.grab_texture_pointer(texture_id);

        if (t  == nullptr) {
            std::cout << "texture is invalid" << std::endl;
        }

        DrawTextureEx(*t, {(pointer.x + (chnk_pos.x * CHUNK_SIZE))*BLOCK_SIZE, (pointer.y + (chnk_pos.y * CHUNK_SIZE))*BLOCK_SIZE}, 0, 1,  WHITE);

    }
}


void render::render_world() {
    draw_chunk({0, 0});
    draw_chunk({1, 1});
    draw_chunk({2, 2});

}


void render::update() {
    camera.zoom = 3;

    BeginMode2D(camera);

    render_world();

    EndMode2D();

    //std::cout << "camera pos.x = " << camera.target.x << ", camera pos.y = " << camera.target.y << std::endl;

}