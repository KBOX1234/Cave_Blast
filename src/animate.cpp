#include "../include/animate.hpp"

int animated_sprite_linker::find_animation_index_by_name(std::string name){
    for(int i = 0; i < animations.size(); i++){
        if(animations[i].name == name){
            return i;
        }
    }

    return -1;
}


