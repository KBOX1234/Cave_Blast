#include "../include/npcs.hpp"

void npc_scripts::test(void* obj){
    npc* npc_obj = (npc*)obj;

    //printf("coords: %d, %d\n", npc_obj->pos->x, npc_obj->pos->y);

    //npc_obj->pos->x++;

    npc_obj->set_rotation(-90);

    npc_obj->move();
}
