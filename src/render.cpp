#include "../include/render.hpp"

#include "player.hpp"

#define BLOCK_SIZE 32

render::render() {
    camera.target = {1, 1};
}


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

void render::update_drawing_coords() {

    camera.target.x = player_manager.host->get_pos().x - ((((float)SCREEN_WIDTH / camera.zoom) / 2.0f) - (BLOCK_SIZE / 2));
    camera.target.y = player_manager.host->get_pos().y - ((((float)SCREEN_HEIGTH / camera.zoom) / 2.0f) - BLOCK_SIZE);

    std::cout << "player pos: " << std::to_string(player_manager.host->get_pos().x) << ", " << std::to_string(player_manager.host->get_pos().y) << std::endl;
    std::cout << "camera pos: " << std::to_string(camera.target.x) << ", " << std::to_string(camera.target.y) << std::endl;



    int cx = (int)floor(camera.target.x / (BLOCK_SIZE * CHUNK_SIZE));
    int cy = (int)floor(camera.target.y / (BLOCK_SIZE * CHUNK_SIZE));

    //std::cout << "camera.target = {" << std::to_string(camera.target.x) << ", " << std::to_string(camera.target.y) << "}" << std::endl;
    //std::cout << "cx = " << std::to_string(cx) << ", "  << std::to_string(cy) << std::endl;

    chunks_pos_to_draw[0] = {static_cast<float>(cx), static_cast<float>(cy)};
    chunks_pos_to_draw[1] = {static_cast<float>(cx + 1), static_cast<float>(cy)};
    chunks_pos_to_draw[2] = {static_cast<float>(cx - 1), static_cast<float>(cy)};

    chunks_pos_to_draw[3] = {static_cast<float>(cx), static_cast<float>(cy + 1)};
    chunks_pos_to_draw[4] = {static_cast<float>(cx + 1), static_cast<float>(cy + 1)};
    chunks_pos_to_draw[5] = {static_cast<float>(cx - 1), static_cast<float>(cy + 1)};

    chunks_pos_to_draw[6] = {static_cast<float>(cx), static_cast<float>(cy - 1)};
    chunks_pos_to_draw[7] = {static_cast<float>(cx + 1), static_cast<float>(cy - 1)};
    chunks_pos_to_draw[8] = {static_cast<float>(cx - 1), static_cast<float>(cy - 1)};

    chunks_pos_to_draw[9] = {static_cast<float>(cx + 2), static_cast<float>(cy)};
    chunks_pos_to_draw[10] = {static_cast<float>(cx + 2), static_cast<float>(cy + 1)};
    chunks_pos_to_draw[11] = {static_cast<float>(cx + 2), static_cast<float>(cy - 1)};

}

void render::draw_chunk(Vector2 chnk_pos) {
    //std::cout << "\ndrawing chunk " << std::to_string(chnk_pos.x) << " " << std::to_string(chnk_pos.y) << std::endl;

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
        float texture_scale = BLOCK_SIZE / (float)t->width;
        //std::cout << "\ntexture scale: " << std::to_string(texture_scale) + ", Height is: " << std::to_string(t->height) << ", BLOCK_SIZE is: " << std::to_string(BLOCK_SIZE)<< std::endl;

        DrawTextureEx(*t, {(pointer.x + (chnk_pos.x * CHUNK_SIZE))*BLOCK_SIZE, (pointer.y + (chnk_pos.y * CHUNK_SIZE))*BLOCK_SIZE}, 0, texture_scale,  WHITE);

    }
}




void render::render_world() {

    for (int i = 0; i < 12; i++) {
        //std::cout << "i = " + std::to_string(i) + ", chunk_pos_to_draw = {" + std::to_string(chunks_pos_to_draw[i].x) + ", " + std::to_string(chunks_pos_to_draw[i].y) + "}\n";
        draw_chunk(chunks_pos_to_draw[i]);
    }

    player_master::draw_player(player_manager.host);
    //draw_chunk({0, 0});

}


void render::update() {
    camera.zoom = 3;

    BeginMode2D(camera);

    update_drawing_coords();

    render_world();

    EndMode2D();

    //std::cout << "camera pos.x = " << camera.target.x << ", camera pos.y = " << camera.target.y << std::endl;

}