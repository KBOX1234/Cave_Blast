#include "../../include/npcs.hpp"

void npc_scripts::rattle_bones(void *obj) {
    npc* npc_obj = (npc*)obj;
    const float aggro = 128;

    if (npc_obj->data_bank.does_object_exist("is_loaded")) {
        npc_obj->data_bank.add_entry(true, "is_loaded");

    }

    player* pp = npc_obj->closest_player();

    if (npc_obj->distance_to_player(pp) < aggro) {
        npc_obj->face_player(pp);
        npc_obj->move();
    }

}
