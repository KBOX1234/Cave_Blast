#include "../include/animate.hpp"
#include "json.hpp"
#include "../include/texture_master.hpp"
#include "../include/delta_time.hpp"
using json = nlohmann::json;

int animated_sprite_linker::find_animation_index_by_name(std::string name){
    for(int i = 0; i < animations.size(); i++){
        if(animations[i].name == name){
            return i;
        }
    }

    return -1;
}

void animated_sprite_linker::load_animation(animated_sprite ams){
    animations.push_back(ams);
}

void animated_sprite_linker::load_animation_from_json(std::string jsonS){
    json j = json::parse(jsonS);

    animated_sprite ams;

    ams.frame_rate = j.value("frame_rate", 60);
    ams.name = j.value("name", "unknown");

    json texture_files = j["textures"];


    for(int i = 0; i < texture_files.size(); i++){
        int texture_id = texture_manager.add_texture(texture_files[i], true);

        Texture2D* txt = texture_manager.grab_texture_pointer(texture_id);

        ams.textures.push_back(txt);
        ams.texture_ids.push_back(texture_id);
    }

    load_animation(ams);

}

void animated_sprite_linker::play_animation(std::string name, bool loop){
    current_animation_index = find_animation_index_by_name(name);

    looping = loop;

    current_animation_age = 0;

    playback_status = PLAY;
}

void animated_sprite_linker::pause_animation(){
    playback_status = PAUSE;
}

void animated_sprite_linker::stop_animation(){
    playback_status = STOP;

}

void animated_sprite_linker::update_animation(){
    if(playback_status == PLAY){
        //sombody fix this
        float current_frame = (float)current_animation_age / (60.0f / (float)animations[current_animation_index].frame_rate);

        int current_frame_int = (int)round(current_frame);

        if(current_frame_int < animations[current_animation_index].textures.size() && pos != nullptr){
            if(animations[current_animation_index].textures[current_frame_int] != nullptr){
                DrawTextureV(*animations[current_animation_index].textures[current_frame_int], *pos, WHITE);
            }

            if(delta_time_master.can_game_continue() == true){
                current_animation_age++;
            }
        }
        else{
            if(looping == true){
                current_animation_age = 0;
            }
            else{
                playback_status = STOP;
            }
        }
    }

    else if(playback_status == PAUSE){
               //sombody fix this
        float current_frame = (float)current_animation_age / (60.0f / (float)animations[current_animation_index].frame_rate);

        int current_frame_int = (int)round(current_frame);
        if(animations[current_animation_index].textures[current_frame_int] != nullptr && pos != nullptr){
            DrawTextureV(*animations[current_animation_index].textures[current_frame_int], *pos, WHITE);
        }


    }
}

void animated_sprite_linker::set_pos(Vector2* poss){
    pos = poss;
}

void animation_master::update_all(){

    for(int i = 0; i < ams_linkers.size(); i++){
        ams_linkers[i]->update_animation();
    }
    
}

void animation_master::link_ams_linker(animated_sprite_linker* ams){
    ams_linkers.push_back(ams);
}


