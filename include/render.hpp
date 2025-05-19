#include "raylib.h"
#include "world.hpp"
#include "texture_master.hpp"
#include <cmath>

class render {
private:

    Camera2D camera;

    Vector2 chunks_pos_to_draw[12];

    void update_drawing_coords();

    void draw_chunk(Vector2 chnk_pos);

    void render_world();



public:

    void set_camera_target(Vector2 pos);

    void move_camera_x(float amount);

    void move_camera_y(float amount);

    void set_camera_zoom(float zoom);

    void update();
};

extern render render_master;