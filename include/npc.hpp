#pragma once

#include "raylib.h"
#include "animate.hpp"
#include "sphysics.h"

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

        animated_sprite_linker amsl;

        animated_transform_linker amtl;

        npc_object();

        void update_colide_box();

        bool does_npc_colide_with(colideBox* other_box);
};

struct npc_status{
    bool is_aggro;
};

struct npc_cheif_end{
    bool (* cheif_end_to_exicute)(npc_status* status, npc_object* npc_obj);
    std::string description;
};

class cheif_end_manager{
      
};
