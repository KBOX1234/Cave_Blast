#include "./../include/partical.hpp"
#include "../include/delta_time.hpp"
#include "../include/texture_master.hpp"

#include <cmath>

bool partical_system::is_partical_expired(partical_cluster* pc){
    if(pc->existance >= pc->lifetime){
        return true;
    }

    else{
        if(delta_time_master.can_game_continue() == true){
            pc->existance++;
        }

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
    for(int i = 0; i < pc->members.size(); i++){
        Texture2D* txt = texture_manager.grab_texture_pointer(pc->texture_id);

        DrawTextureV(*txt, pc->members[i], pc->tint);
    }
}


