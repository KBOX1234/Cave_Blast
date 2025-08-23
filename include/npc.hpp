#pragma once

#include "raylib.h"
#include "animate.hpp"
#include "sphysics.h"
#include "inventory.hpp"
#include <vector>

class npc;

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

        animated_sprite_linker* amsl;

        animated_transform_linker* amtl;

        npc_object();
        ~npc_object();
        void update_colide_box();

        bool does_npc_colide_with(colideBox* other_box);
};

class npc_scripts;

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
    friend class npc_scripts;
    friend class npc_master;
    private:
        //this is realy cursed
        void* myself;

        npc_object* npc_data;

        Vector2* pos;

        float* rotation;

        float* scale;

        Vector2* size;

        void (*npc_cheif_end)(void* npc_ptr) = nullptr;

        int id;

        int texture_id;

        npc_stats stat;

        Texture2D* cache;

    public:
        npc();

        ~npc();
        void assign_my_pointer(void* ptr);

        void draw();
};

struct npc_template{
    std::string name;

    Vector2 size;
    npc_stats stat;

    int texture_id;

    void (*npc_cheif_end)(void* npc_ptr) = nullptr;
};

class npc_template_loader{
    private:
        std::vector<npc_template> templates;

        std::vector<int> find_duplicate_named_templates(std::string name);

    public:

        bool load_template(npc_template npct, bool remove_duplicate = false);

        npc_template* get_npc_template(std::string name);

        bool load_templates_from_json(std::string path);

        
       
};

extern npc_template_loader npc_template_manager;

class render;

class npc_master {
    friend class render;
    private:
        std::vector<npc*> npcs;

        void draw_npcs();

    public:

        void update_npcs();

        int new_npc(std::string npc_type, Vector2 pos);

        bool remove_npc(int id);
};

extern npc_master npc_manager;
