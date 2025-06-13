#include "raylib.h"
#include <vector>

struct light{
    Color color;

    float radius;

    Vector2 pos;

    float strength;

    bool skip;
};

class render;

class light_master{
    friend class render;

    private:

        std::vector<light> lights;

        RenderTexture2D generate_lights(Vector2 viewport_pos, Vector2 viewport_size);

     public:

        int add_light(Color color, float radius, Vector2 pos, float strength, long frames_till_expire);

        void update_light_pos(int index, Vector2 pos);

        void update_light_color(int index, Color color);

        void update_light_radius(int index, float radius);

        void update_light_strength(int index, float strength);

        void remove_light(int index);

};

extern light_master light_manager;