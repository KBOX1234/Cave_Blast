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


void render::render_world() {
    Vector2 chnk_pos;

    /*chnk_pos.x = camera.target.x /  BLOCK_SIZE;
    chnk_pos.y = camera.target.y / BLOCK_SIZE;

    chnk_pos.x = chnk_pos.x / BLOCK_SIZE;
    chnk_pos.y = chnk_pos.y / BLOCK_SIZE;*/

    chnk_pos.x = 0;
    chnk_pos.y = 0;

    chnk_pos.x = round(chnk_pos.x);
    chnk_pos.y = round(chnk_pos.y);

    const block* render_chnk = world.chunk_buffer(chnk_pos);
    Vector2 pointer = {0, 0};

    for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++) {
        if (render_chnk == nullptr) {
            std::cout << "somthing went wrong" << std::endl;
            break;
        }

        if (pointer.x > CHUNK_SIZE) {
            pointer.x = 0;
            pointer.y++;
        }
        //std::cout << "pointer.x = " << pointer.x << ", pointer.y = " << pointer.y << std::endl;
        //*texture_manager.grab_texture_pointer(render_chnk[i].attr->texture_id)
        //DrawTexture(*texture_manager.grab_texture_pointer(texture_manager.default_texture()), pointer.x*BLOCK_SIZE, pointer.y*BLOCK_SIZE, WHITE);
        //std::cout << std::to_string(render_chnk[i].attr->texture_id) << ", default texture id: " + std::to_string(texture_manager.default_texture()) + " \n";

        int texture_id = render_chnk[i].attr->texture_id;

        Texture2D* t = texture_manager.grab_texture_pointer(texture_id);

        if (t  == nullptr) {
            std::cout << "texture is invalid" << std::endl;
        }

        DrawTextureEx(*t, {pointer.x*BLOCK_SIZE, pointer.y*BLOCK_SIZE}, 0, 1,  WHITE);
        pointer.x++;

    }
}


void render::update() {
    camera.zoom = 3;

    BeginMode2D(camera);

    render_world();

    EndMode2D();

    //std::cout << "camera pos.x = " << camera.target.x << ", camera pos.y = " << camera.target.y << std::endl;

}