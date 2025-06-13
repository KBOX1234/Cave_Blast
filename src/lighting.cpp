#include "../include/lighting.hpp"
#include <math.h>


template<typename T>
inline T Clamp(T value, T min, T max) {
    if (value < min) return min;
    else if (value > max) return max;
    else return value;
}

RenderTexture2D light_master::generate_lights(Vector2 viewport_pos, Vector2 viewport_size){

    RenderTexture2D rnd = LoadRenderTexture((int)viewport_size.x, (int)viewport_size.y);

    BeginTextureMode(rnd);

    ClearBackground(BLACK);

    BeginBlendMode(BLEND_ADDITIVE);

    for(int i = 0; i < lights.size(); i++){
        if(lights[i].pos.x > viewport_pos.x && lights[i].pos.x < viewport_pos.x + viewport_size.x && lights[i].pos.y > viewport_pos.y && lights[i].pos.y < viewport_pos.y + viewport_size.y && lights[i].skip == false){
            Vector2 img_light_pos;

            img_light_pos.x = lights[i].pos.x - viewport_pos.x;
            img_light_pos.y = lights[i].pos.y - viewport_pos.y;

            DrawCircleGradient(img_light_pos.x, img_light_pos.y, lights[i].radius, WHITE, BLACK);


        }
    }
    EndBlendMode();
    EndTextureMode();

    return rnd;
}

int light_master::add_light(Color color, float radius, Vector2 pos, float strength, long frames_till_expire){
    light new_light;

    new_light.color = color;
    new_light.radius = radius;
    new_light.pos = pos;
    new_light.strength = strength;

    new_light.skip = false;

    lights.push_back(new_light);

    return lights.size() - 1;
}

void light_master::update_light_pos(int index, Vector2 pos){
    if(index >= lights.size()) return;

    lights[index].pos = pos;
}

void light_master::update_light_color(int index, Color color){
    if(index >= lights.size()) return;

    lights[index].color = color;
}

void light_master::update_light_radius(int index, float radius){
    if(index >= lights.size()) return;

    lights[index].radius = radius;
}

void light_master::update_light_strength(int index, float strength){
    if(index >= lights.size()) return;

    lights[index].strength = strength;
}

void light_master::remove_light(int index){
    lights[index].skip = true;
}