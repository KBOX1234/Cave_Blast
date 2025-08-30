#include "../include/animate.hpp"
#include "json.hpp"
#include "../include/texture_master.hpp"
#include "../include/delta_time.hpp"
using json = nlohmann::json;

std::string animated_sprite_linker::get_current_animation_name() {
    if (playback_status != STOP) {
        if (playback_status == PLAY || playback_status == PAUSE) {
            return animations[current_animation_index].name;
        }
    }

    return "NO_ANIMATION_ACTIVE";
}

int animated_sprite_linker::get_playback_status() {
    return playback_status;
}

int animated_sprite_linker::get_current_animation_age() {
    if (playback_status != STOP) {
        if (playback_status == PLAY || playback_status == PAUSE) {
            return current_animation_age;
        }
    }

    return NO_ANIMATION_ACTIVE;
}




int animated_sprite_linker::find_animation_index_by_name(std::string name){
    std::string truncated = name.substr(0, MAX_ANIMATE_NAME_LENGTH);
    name = truncated;
    for(int i = 0; i < animations.size(); i++){
        if(animations[i].name == name){
            return i;
        }
    }

    return -1;
}

void animated_sprite_linker::load_animation(animated_sprite ams){
    std::string truncated = ams.name.substr(0, MAX_ANIMATE_NAME_LENGTH);
    ams.name = truncated;
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
    if (j.contains("offset")) {
        json offset = j["offset"];

        ams.offset.x = offset.value("x", 0);
        ams.offset.y = offset.value("y", 0);
    }

    load_animation(ams);

}

void animated_sprite_linker::play_animation(std::string name, bool loop){
    std::string truncated = name.substr(0, MAX_ANIMATE_NAME_LENGTH);
    name = truncated;

    current_animation_index = find_animation_index_by_name(name);

    if(current_animation_index == -1){
        std::cout << "(ANIMATED_SPRITE_LINKER): Could not find animation\n";
        return;
    }

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

    if (current_animation_index < 0 || current_animation_index >= animations.size()) return;

    if(playback_status == PLAY){
        //sombody fix this
        float current_frame = (float)current_animation_age / (60.0f / (float)animations[current_animation_index].frame_rate);

        int current_frame_int = (int)round(current_frame);

        if(current_frame_int < animations[current_animation_index].textures.size() && current_frame_int >= 0){
            if(animations[current_animation_index].textures[current_frame_int] != nullptr && pos != nullptr && animations[current_animation_index].textures[current_frame_int]->id != 0){
                Vector2 offset_pos;
                offset_pos.x = pos->x + animations[current_animation_index].offset.x;
                offset_pos.y = pos->y + animations[current_animation_index].offset.y;
                DrawTextureV(*animations[current_animation_index].textures[current_frame_int], offset_pos, WHITE);
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
        if(animations[current_animation_index].textures[current_frame_int] != nullptr && pos != nullptr && current_frame_int < animations[current_animation_index].textures.size() && current_frame_int >= 0){
            DrawTextureV(*animations[current_animation_index].textures[current_frame_int], *pos, WHITE);
        }


    }
}

void animated_sprite_linker::set_pos(Vector2* poss){
    pos = poss;
}

int animated_transform_linker::find_animation_index_by_name(std::string name){
    for(int i = 0; i < animations.size(); i++){
        if(animations[i].name == name){
            return i;
        }
    }

    return -1;
}

void animated_transform_linker::link_pointers(float* scaleS, float* rotationR, Vector2* posP){
    scale = scaleS;

    rotation = rotationR;

    pos = posP;

}

void animated_transform_linker::update_animation(){
if (animations.empty() || current_animation_index < 0 || current_animation_index >= animations.size())
    return;


    if(playback_status == PLAY && delta_time_master.can_game_continue() == true){
        Vector2 move_amount;
        Vector2 distance;
        float scale_distance;
        float scale_amount;
        float rotate_distance;
        float rotate_amount;

        animated_transform amt_current = animations[current_animation_index];

        if(amt_current.interpolate_from_target_state == true){
            distance.x = amt_current.pos2.x - pos->x;
            distance.y = amt_current.pos2.y - pos->y;

            if(scale != nullptr) scale_distance = amt_current.scale2 - *scale;
            if(rotation != nullptr) rotate_distance = amt_current.rotation2 - *rotation;
        }

        else{
            distance.x = amt_current.pos2.x - amt_current.pos1.x;
            distance.y = amt_current.pos2.y - amt_current.pos1.y;

            scale_distance = amt_current.scale2 - amt_current.scale1;
            rotate_distance = amt_current.rotation2 - amt_current.rotation1;
        }

        if (amt_current.lifetime > 0) {
            move_amount.x = distance.x / amt_current.lifetime;
            move_amount.y = distance.y / amt_current.lifetime;
            rotate_amount = rotate_distance / amt_current.lifetime;
            scale_amount = scale_distance / amt_current.lifetime;
        } 
        else {
            // handle error or fallback
        }


        if(amt_current.lifetime > current_animation_age){

            if(pos != nullptr){
                pos->x = pos->x + move_amount.x;
                pos->y = pos->y + move_amount.y;
            }
            if(rotation != nullptr){
                *rotation = *rotation + rotate_amount;
            }
            if(scale != nullptr){
                *scale = *scale + scale_amount;
            }
            current_animation_age++;
        }

        else playback_status = STOP;
    
    }

    if(playback_status == PAUSE){
        //idk
    }

    if(playback_status == STOP){
        //idk
    }
    
   

}

void animated_transform_linker::load_animation(animated_transform amt){
    animations.push_back(amt);
}

void animated_transform_linker::load_animation_from_json(std::string jsonJ){
    animated_transform amt;

    json j = json::parse(jsonJ);

    amt.scale1 = j.value("scale1", 1);
    amt.rotation1 = j.value("rotation1", 0);
    if(j.contains("pos1") == true){
        amt.pos1.x = j["pos1"]["x"];
        amt.pos1.y = j["pos1"]["y"];
    }

    amt.scale2 = j.value("scale2", 1);
    amt.rotation2 = j.value("rotation2", 0);

    if(j.contains("pos2") == false){
        std::cout << "(ANIMATED_TRANSFORM_LINKER): Animation does not contain pos2\n";
        return;
    }

    amt.pos2.x = j["pos2"]["x"];
    amt.pos2.y = j["pos2"]["y"];

    amt.lifetime = j.value("lifetime", 60);
    amt.interpolate_from_target_state = j.value("interpolate_from_target_state", false);
    amt.name = j.value("name", "unknown");

    animations.push_back(amt);
    
}

void animated_transform_linker::play_animation(std::string name){
    int res = find_animation_index_by_name(name);

    if(res == -1) {
        std::cout << "(ANIMATED_TRANSFORM_LINKER): Animation not found\n";
        return;
    }

    current_animation_index = res;

    current_animation_age = 0;

    playback_status = PLAY;


    if(animations[res].interpolate_from_target_state == false){
        *pos = animations[res].pos1;
        *scale = animations[res].scale1;
        *rotation = animations[res].rotation1;
    }

}

void animated_transform_linker::pause_animation(){
    playback_status = PAUSE;
}

void animated_transform_linker::stop_animation(){
    playback_status = PAUSE;
}

void animation_master::update_all(){

    for(int i = 0; i < ams_linkers.size(); i++){
        if(ams_linkers[i] != nullptr){
            ams_linkers[i]->update_animation();
        }
        //else std::cout << "ams linker " + std::to_string(i) + " was nullptr\n";
    }
    for(int i = 0; i < amt_linkers.size(); i++){

        if(amt_linkers[i] != nullptr) amt_linkers[i]->update_animation();
    }  
}

void animation_master::link_ams_linker(animated_sprite_linker* ams){
    ams_linkers.push_back(ams);
}

void animation_master::link_amt_linker(animated_transform_linker* amt){
    amt_linkers.push_back(amt);
}


