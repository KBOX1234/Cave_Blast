#pragma once

#include "raylib.h"
#include "animate.hpp"
#include "sphysics.h"
#include "inventory.hpp"
#include <vector>
#include "player.hpp"
#include "isb.hpp"

#define NPC_TYPE_STR_SIZE 20

class npc;
class npc_scripts;
//npc object is the shell of an npc which defines the basic data needed to represent it
class npc_object{
    friend class npc;
    private:
        Vector2 pos;

        float rotation;

        float scale;

        Vector2 size;

        colideBox box;


    public:

        animated_sprite_linker amsl;

        animated_transform_linker amtl;

        npc_object();
        
        void update_colide_box();

        bool does_npc_colide_with(colideBox* other_box);
};



struct npc_stats{
    int health;
    int max_health;

    int max_damage;
    int max_block;

    float speed;
    bool target_player;

};

class npc_master;

class npc{
    //friend class npc_scripts;
    friend class npc_master;
    private:
        //this is realy cursed

        std::string template_name;

        void* myself;

        Vector2* pos;

        float* rotation;

        float* scale;

        Vector2* size;

        void (*npc_cheif_end)(void* npc_ptr) = nullptr;

        int id;

        int texture_id;

        npc_stats stat;

        animated_sprite_linker* amsl;

        animated_transform_linker* amtl;

        Texture2D* cache;

        Vector2 last_pos;

        isb data_bank;

        Vector2 get_move_target();

        bool is_valid_move_2(Vector2 pos2);

        void update_time_stamp();

    public:

        npc_object* npc_data;
        npc();

        ~npc();
        void assign_my_pointer(void* ptr);

        void draw();

        void set_rotation(float rot);

        void add_rotation(float rot);

        void move();

        player* closest_player();

        Vector2 get_pos();

        void face_player();
};

struct npc_template{
    std::string name;

    Vector2 size;
    npc_stats stat;

    int texture_id;

    std::vector<std::string> animation_jsons;

    //npc_ptr is the npc object
    void (*npc_cheif_end)(void* npc_ptr) = nullptr;
};



struct serialized_npc {
    Vector2 pos;

    float rotation;

    float scale;

    Vector2 size;

    npc_stats stat;

    int id;

    char type[NPC_TYPE_STR_SIZE];

    char current_animation[MAX_ANIMATE_NAME_LENGTH];

    int current_animation_age;

    int playback_status;

    bool looping_animation;
};

class npc_template_loader{
    private:
        std::vector<npc_template> templates;

        std::vector<int> find_duplicate_named_templates(std::string name);

    public:

        bool load_template(npc_template npct, bool remove_duplicate = false);

        npc_template* get_npc_template(std::string name);

        bool load_templates_from_json(std::string path);

        void assign_func_to_npc(void (*func)(void* npc_ptr), std::string name);
       
};

extern npc_template_loader npc_template_manager;

class render;
class server_utls;

class npc_master {
    friend class render;
    friend class server_utls;
    private:
        std::vector<npc*> npcs;

        void draw_npcs();

        int find_npc_slot_by_id(int id);

        int new_npc_from_serialized_npc(serialized_npc npc_c);

    public:

        serialized_npc serialize_npc(npc* npc_c);

        void update_npc_from_serialized_npc(serialized_npc npc_c);

        void update_npcs();

        int new_npc(std::string npc_type, Vector2 pos);

        bool remove_npc(int id);
};

extern npc_master npc_manager;
