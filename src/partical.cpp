#include "./../include/partical.hpp"
#include "../include/delta_time.hpp"

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


