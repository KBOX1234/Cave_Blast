#pragma once

#include "raylib.h"
#include <iostream>
#include <vector>

#define PLAY 0
#define PAUSE 1
#define STOP 2

#define SPRITE 0
#define TRANSLATION 1

struct animated_transform{
    //the original state to translate from
    float scale1;
    float rotation1;
    Vector2 pos1;

    //the state to translate from
    float scale2;
    float rotation2;
    Vector2 pos2;
    
    //the amount of frames to run this animation
    int frames;
    
    //the targets state as a pointer
    float* target_scale;
    float* target_rotation;
    Vector2* target_pos;
    
    //if set to true, it will ignore scale1, rotation1, and pos1 
    bool interpolate_from_target_state;

    //
};

struct animated_sprite{
    std::vector<Texture2D*> textures;
    std::vector<int> texture_ids;

    int frame_rate;
    std::string name;
};

class animation_master;


//each object that wants to be animated has this
//its in charge of everything animated exept for drawing
class animated_sprite_linker{
    friend class animation_master;
    
    private:

        Vector2* pos;

        std::vector<animated_sprite> animations;

        int find_animation_index_by_name(std::string name);

        int current_animation_index;

        int current_animation_age;

        int playback_status;

        bool looping = false;

        void update_animation();
        
    public:
                
        void load_animation(animated_sprite ams);

        void load_animation_from_json(std::string jsonS);

        void play_animation(std::string name, bool loop = false);

        void pause_animation();

        void stop_animation();
        
        void set_pos(Vector2* poss);
};

//this class is in charge of polling all the animated frames to be drawing
//This is to allow for drawing things all at once
//The last shall be first and the first shall be last
//

class render;

class animation_master{
    friend class render;
    private:
        std::vector<animated_sprite_linker*> ams_linkers;  

        void update_all();


    public:

        void link_ams_linker(animated_sprite_linker* ams);
};

extern animation_master animation_manager;
