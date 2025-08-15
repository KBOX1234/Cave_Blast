#pragma once

#include "raylib.h"
#include "animate.hpp"
#include "sphysics.h"

class npc_object{
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
