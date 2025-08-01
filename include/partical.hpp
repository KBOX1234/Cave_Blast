#pragma once
#include "raylib.h"
#include <vector>
#include <iostream>


struct partical_cluster{

    Vector2 origin;
    
    //what texture should be used
    int texture_id;

    //how many particles
    int partical_count;

    //lifetime stored in frames
    int lifetime;

    //how long it has existed
    int existance;

    //how fast should these particles explode
    float velocity;

    //max distance it can spawn from where it has been spawned;
    float spawn_range;
   
    //particle members
    std::vector<Vector2> members;   
    std::vector<float> member_directions;

    Color tint;

    
};

struct partical_preset{
    int texture_id;

    int partical_count;

    int lifetime;

    float velocity;

    float spawn_range;

    std::string name;

    Color tint;
};

class partical_system{

    private:
        std::vector<partical_cluster> particles;
        std::vector<partical_preset> presets;

        bool is_partical_expired(partical_cluster* pc);
        void simulate_partical_cluster(partical_cluster* pc);
        void draw_partical(partical_cluster* pc);


        void load_partical_presets_from_json_file(std::string path);

    public:
        
        void init();

        void update();

        void spawn_partical_custome(partical_preset pp, Vector2 pos);

        void spawn_partical(std::string name);
        
    
};

extern partical_system partical_manager;
