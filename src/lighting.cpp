#include "lighting.hpp"
#include <math.h>

template<typename T>
inline T Clamp(T value, T min, T max) {
    if (value < min) return min;
    else if (value > max) return max;
    else return value;
}


void light_master::DrawLightCircleToImage(Image *img, Vector2 center, float radius, Color color, float intensity){
    int imgWidth = img->width;
    int imgHeight = img->height;

    int startX = (int)(center.x - radius);
    int startY = (int)(center.y - radius);
    int endX   = (int)(center.x + radius);
    int endY   = (int)(center.y + radius);

    for (int y = startY; y <= endY; y++)
    {
        if (y < 0 || y >= imgHeight) continue;

        for (int x = startX; x <= endX; x++)
        {
            if (x < 0 || x >= imgWidth) continue;

            float dx = x - center.x;
            float dy = y - center.y;
            float dist = sqrtf(dx*dx + dy*dy);

            if (dist <= radius)
            {
                float alphaFactor = 1.0f - (dist / radius);
                alphaFactor = powf(alphaFactor, intensity); // smoother falloff

                unsigned char a = (unsigned char)(color.a * alphaFactor);
                Color result = {
                    (unsigned char)(color.r * alphaFactor),
                    (unsigned char)(color.g * alphaFactor),
                    (unsigned char)(color.b * alphaFactor),
                    (unsigned char)(color.a * alphaFactor)
                };

                // Blend with existing pixel (additive)
                Color existing = GetImageColor(*img, x, y);
                Color blended = {
                    (unsigned char)Clamp(existing.r + result.r, 0, 255),
                    (unsigned char)Clamp(existing.g + result.g, 0, 255),
                    (unsigned char)Clamp(existing.b + result.b, 0, 255),
                    (unsigned char)Clamp(existing.a + result.a, 0, 255)
                };

                ImageDrawPixel(img, x, y, blended);
            }
        }
    }
}


Texture2D light_master::generate_lights(Vector2 viewport_pos, Vector2 viewport_size){

    Image img = GenImageColor(viewport_size.x, viewport_size.y, BLACK);

    

    for(int i = 0; i < lights.size(); i++){
        if(lights[i].pos.x > viewport_pos.x && lights[i].pos.x < viewport_pos.x + viewport_size.x && lights[i].pos.y > viewport_pos.y && lights[i].pos.y < viewport_pos.y + viewport_size.y && lights[i].skip == false){
            Vector2 img_light_pos;

            img_light_pos.x = lights[i].pos.x - viewport_pos.x;
            img_light_pos.y = lights[i].pos.y - viewport_pos.y;

            DrawLightCircleToImage(&img, img_light_pos, lights[i].radius, lights[i].color, lights[i].strength);
        }
    }

    Texture2D rtv = LoadTextureFromImage(img);

    UnloadImage(img);

    return rtv;
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