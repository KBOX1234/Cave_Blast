#include "npc.hpp"

npc_object::npc_object(){
    animation_manager.link_ams_linker(&amsl);
    animation_manager.link_amt_linker(&amtl);

    pos = {0, 0};

    scale = 1;

    rotation = 0;
}

void npc_object::update_colide_box(){
    
    box.a.x = pos.x;
    box.a.y = pos.y;

    Vector2 scaled_size = {size.x * scale, size.y* scale};

    box.b.x = pos.x + scaled_size.x;
    box.b.y = pos.y + scaled_size.y;
}

bool npc_object::does_npc_colide_with(colideBox* other_box){
    char result = doesBoxAndBoxColide(other_box, &box);

    return result;
}
