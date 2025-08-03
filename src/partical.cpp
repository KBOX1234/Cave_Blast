#include "./../include/partical.hpp"
#include "../include/delta_time.hpp"
#include "../include/texture_master.hpp"
#include "../include/lighting.hpp"

#include <cmath>
#include <random>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

bool partical_system::is_partical_expired(partical_cluster* pc){
    if(pc->existance >= pc->lifetime){
        return true;
    }

    else{
        //if(delta_time_master.can_game_continue() == true){
            pc->existance++;
        

        return false;
    }
}

void partical_system::simulate_partical_cluster(partical_cluster* pc){
    for(int i = 0; i < pc->members.size(); i++){
        float radians = pc->member_directions[i] * (PI / 180.0f);

        Vector2 direction = {
            cosf(radians),
            sinf(radians)
        };

        
        pc->members[i].x = pc->members[i].x + (direction.x * pc->velocity);
        pc->members[i].y = pc->members[i].y + (direction.y * pc->velocity);

    }
    

    pc->spawn_range = pc->spawn_range + pc->velocity;
    light_manager.update_light_radius(pc->light_index, pc->spawn_range);

}

void partical_system::draw_partical(partical_cluster* pc){

    if(delta_time_master.can_game_continue() == true){
        pc->tint.a = pc->tint.a - (pc->tint.a/(float)pc->lifetime);
        //light_manager.update_light_color(pc->light_index, pc->tint);
        pc->light_power = pc->light_power + 10;
        light_manager.update_light_strength(pc->light_index, pc->light_power);
    }


    for(int i = 0; i < pc->members.size(); i++){
        Texture2D* txt = texture_manager.grab_texture_pointer(pc->texture_id);
        DrawTextureV(*txt, pc->members[i], pc->tint);

        //pc->tint.a = pc->tint.a - (255/(float)pc->lifetime);
    }
}

void partical_system::load_partical_presets_from_json_file(std::string path){
    std::ifstream file(path);

    json j;

    if (file.is_open()) {
        file >> j;
    } 

    else {
        std::cerr << "Failed to open " << path << std::endl;
    }

    for(auto& json_item : j){
        partical_preset pp;

        pp.texture_id = texture_manager.add_texture(json_item.value("texture", "reasource/gfx/default.png"), true);
        pp.partical_count = json_item.value("partical_count", 1);
        pp.lifetime = json_item.value("lifetime", 1);
        pp.velocity = json_item.value("velocity", 1);
        pp.spawn_range = json_item.value("spawn_range", 1);
        pp.name = json_item.value("name", "no_name");
        pp.has_light = json_item.value("has_light", false);
        pp.light_power = json_item.value("light_power", 1);

        Color tint = WHITE;
        if (json_item.contains("color") && json_item["color"].is_object()) {
            json color = json_item["color"];

            tint.r = color.value("r", 0); // 0 is default if missing
            tint.g = color.value("g", 0);
            tint.b = color.value("b", 0);
            tint.a = 255;
            //std::cout << "Color RGB: (" << r << ", " << g << ", " << b << ")\n";
        } 
        else {
            //std::cerr << "Color data is missing or not an object.\n";
        }

        pp.tint = tint;

        presets.push_back(pp);
    }

}

void partical_system::init(){
    load_partical_presets_from_json_file("reasource/partical_presets.json");
}

void partical_system::update(){
    for(int i = 0; i < particles.size(); i++){

        draw_partical(&particles[i]);

        if(delta_time_master.can_game_continue() == true){

            bool status = is_partical_expired(&particles[i]);
            simulate_partical_cluster(&particles[i]);

            if(status == true){
                light_manager.remove_light(particles[i].light_index);
                particles.erase(particles.begin() + i);
                i--;
            }

        }
        
    }
    
}

void partical_system::spawn_partical_custome(partical_preset pp, Vector2 pos){
    partical_cluster pc;

    pc.origin = pos;

    pc.texture_id = pp.texture_id;

    pc.partical_count = pp.partical_count;

    pc.lifetime = pp.lifetime;

    pc.velocity = pp.velocity;

    pc.existance = 0;

    pc.spawn_range = pp.spawn_range;

    pc.tint = pp.tint;

    pc.has_light = pp.has_light;

    float spawn_radius = pp.spawn_range / 2;

    pc.light_power = pp.light_power;

    pc.light_index = light_manager.add_light(pc.tint, pp.spawn_range*8, pos, pc.light_power, pc.lifetime);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> dist_pos(
        static_cast<int>(-round(spawn_radius)), 
        static_cast<int>(round(spawn_radius))
    );
    std::uniform_int_distribution<int> dist_angle(0, 360);

    for(int i = 0; i < pp.partical_count; i++){


        float x = (float)dist_pos(gen);
        float y = (float)dist_pos(gen);
        float rot = (float)dist_angle(gen);

        pc.members.push_back({x, y});
        pc.member_directions.push_back(rot);

    }

    particles.push_back(pc);

}

void partical_system::spawn_partical(std::string name, Vector2 pos){
    for(int i = 0; i < presets.size(); i++){
        if(presets[i].name == name){
            spawn_partical_custome(presets[i], pos);

            return;
        }
    }
}
