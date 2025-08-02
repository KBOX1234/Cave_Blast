#include "./../include/partical.hpp"
#include "../include/delta_time.hpp"
#include "../include/texture_master.hpp"
#include "../include/lighting.hpp"

#include <cmath>
#include <random>

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
}

void partical_system::draw_partical(partical_cluster* pc){

    if(delta_time_master.can_game_continue() == true){
        pc->tint.a = pc->tint.a - (pc->tint.a/(float)pc->lifetime);
        light_manager.update_light_color(pc->light_index, pc->tint);
    }


    for(int i = 0; i < pc->members.size(); i++){
        Texture2D* txt = texture_manager.grab_texture_pointer(pc->texture_id);
        DrawTextureV(*txt, pc->members[i], pc->tint);

        //pc->tint.a = pc->tint.a - (255/(float)pc->lifetime);
    }
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

    pc.light_index = light_manager.add_light(pc.tint, pp.spawn_range*2, pos, 1, pc.lifetime);

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
