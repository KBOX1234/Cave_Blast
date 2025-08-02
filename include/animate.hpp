#include "raylib.h"
#include <iostream>
#include <vector>

#define PLAY 0
#define PAUSE 1
#define STOP 2

#define SPRITE 0
#define TRANSLATION 1


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

        animation_master* am_pointer;

        std::vector<animated_sprite> animations;

        std::string asl_name;

        int find_animation_index_by_name(std::string name);

        int current_animation_index;

        int current_animation_age;

        int playback_status;
        
    public:
        
        void assign_name(std::string name);
        
        void load_animation(animated_sprite am, std::string name);

        void load_animation_from_json(std::string json);

        void play_animation(std::string name, bool loop = false);

        void pause_animation();

        void stop_animation();
};

//this class is in charge of polling all the animated frames to be drawing
//This is to allow for drawing things all at once
//The last shall be first and the first shall be last
class animation_master{
    private:

        std::vector<Texture2D*> frames;

        void flush_all_frames();


    public:

        void poll_frame(Texture2D* txt);
};

extern animation_master animation_manager;
