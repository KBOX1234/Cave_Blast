#pragma once
#include "raylib.h"
#include <cmath>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGTH 720

class render {
private:

    Camera2D camera;

    Vector2 chunks_pos_to_draw[12];

    void update_drawing_coords();

    void draw_chunk(Vector2 chnk_pos);

    void render_world();

    void render_players(bool debug = false);

    void render_lights();



public:
    render();

    void set_camera_target(Vector2 pos);

    void move_camera_x(float amount);

    void move_camera_y(float amount);

    void set_camera_zoom(float zoom);

    void draw_cursor(Vector2 pos);

    Vector2 get_camera_pos();

    float get_camera_zoom();

    void update();
};

extern render render_master;