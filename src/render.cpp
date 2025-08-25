#include "render.hpp"

#include "player.hpp"
#include "input.hpp"
#include "world.hpp"
#include "texture_master.hpp"
#include "lighting.hpp"
#include "partical.hpp"
#include "../include/animate.hpp"
#include "npc.hpp"

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

    camera.target.x = player_manager.myself->get_pos().x - ((((float)GetScreenWidth() / camera.zoom) / 2.0f) - (BLOCK_SIZE / 2));
    camera.target.x = round(camera.target.x);
    camera.target.y = player_manager.myself->get_pos().y - ((((float)GetScreenHeight() / camera.zoom) / 2.0f) - BLOCK_SIZE);
    camera.target.y = round(camera.target.y);

    //std::cout << "player pos: " << std::to_string(player_manager.get_host()->get_pos().x) << ", " << std::to_string(player_manager.get_host()->get_pos().y) << std::endl;
    //std::cout << "camera pos: " << std::to_string(camera.target.x) << ", " << std::to_string(camera.target.y) << std::endl;



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

    if (render_chnk == nullptr) return;

    for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++) {

        if (pointer.x >= CHUNK_SIZE) {
            pointer.x = 0;
            pointer.y++;
        }
        pointer.x++;

        if (render_chnk[i].attr == nullptr) continue;

        int texture_id = render_chnk[i].attr->texture_id;

        Texture2D* t = texture_manager.grab_texture_pointer(texture_id);

        if (t  != nullptr) {

            if (t->id != 0) {
                float texture_scale = BLOCK_SIZE / (float)t->width;
                //std::cout << "\ntexture scale: " << std::to_string(texture_scale) + ", Height is: " << std::to_string(t->height) << ", BLOCK_SIZE is: " << std::to_string(BLOCK_SIZE)<< std::endl;

                DrawTextureEx(*t, {(pointer.x + (chnk_pos.x * CHUNK_SIZE))*BLOCK_SIZE, (pointer.y + (chnk_pos.y * CHUNK_SIZE))*BLOCK_SIZE}, 0, texture_scale,  WHITE);
            }

        }
        else std::cout << "(RENDER): Texture is null, skipping" << std::endl;


    }
}




void render::render_world() {

    for (int i = 0; i < 12; i++) {
        //std::cout << "i = " + std::to_string(i) + ", chunk_pos_to_draw = {" + std::to_string(chunks_pos_to_draw[i].x) + ", " + std::to_string(chunks_pos_to_draw[i].y) + "}\n";
        draw_chunk(chunks_pos_to_draw[i]);
    }

    //player_manager.draw_player(player_manager.get_host());
    //draw_chunk({0, 0});

}

void render::draw_cursor(Vector2 pos) {
    DrawRectangleLines(pos.x*BLOCK_SIZE, pos.y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, WHITE);
}

void render::render_players(bool debug) {
    for (int i = 0; i < player_manager.players.size(); i++) {

        if (player_manager.host_id != player_manager.players[i]->get_id()) {
            player_manager.draw_player(player_manager.players[i]);
        }

        else if (debug == true){
            DrawRectangleLines(player_manager.players[i]->get_pos().x, player_manager.players[i]->get_pos().y, PLAYER_WIDTH, PLAYER_HEIGHT, RED);
        }

    }
}



void render::update() {
    camera.zoom = 3;

    Vector2 offset = {camera.target.x - (GetScreenWidth() / 2), camera.target.y - (GetScreenHeight() / 2)};

    light_manager.generate_lights(offset, {(float)GetScreenWidth(), (float)GetScreenHeight()});

    light_map = light_manager.light_map;

    BeginMode2D(camera);

    update_drawing_coords();

    //render_world();

    render_players(true);

    animation_manager.update_all();

    player_manager.draw_player(player_manager.myself);

    npc_manager.draw_npcs();

    render_lights();
    
    partical_manager.update();

    draw_cursor(input_manager.cursor);

    EndMode2D();

    //std::cout << "camera pos.x = " << camera.target.x << ", camera pos.y = " << camera.target.y << std::endl;

}

Vector2 render::get_camera_pos() {
    return camera.target;
}

float render::get_camera_zoom() {
    return camera.zoom;
}

void render::render_lights() {
    BeginBlendMode(BLEND_MULTIPLIED);

    float zoom = camera.zoom;
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    // Get top-left corner of the camera view in world coordinates
    Vector2 topLeftWorld = {
        camera.target.x - (screenW / 2),
        camera.target.y - (screenH / 2)
    };

    // raylib render textures are vertically flipped, so we draw them "upside-down"
    // We correct this by shifting the Y coordinate down by the texture height (in world units)
    Vector2 drawPos = {
        camera.target.x - (GetScreenWidth()/2),
        camera.target.y - (GetScreenHeight()/2)
    };

    Vector2 viewport_size = {(float)GetScreenWidth(), (float)GetScreenHeight()};

    Rectangle source = {
        0, 0,
        (float)light_map.texture.width,
        -(float)light_map.texture.height // <-- flips vertically
    };

    Rectangle dest = {
        topLeftWorld.x,
        topLeftWorld.y,
        viewport_size.x,
        viewport_size.y
    };

    DrawTexturePro(light_map.texture, source, dest, { 0, 0 }, 0.0f, WHITE);

    EndBlendMode();

    //UnloadRenderTexture(light_map);
}

