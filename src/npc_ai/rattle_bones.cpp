#include "../../include/npcs.hpp"

void npc_scripts::rattle_bones(void *obj) {
    npc* npc_obj = (npc*)obj;
    const float aggro = 128;

    if (npc_obj->data_bank.does_object_exist("is_loaded")) {
        npc_obj->data_bank.add_entry(true, "is_loaded");

    }


    if (npc_obj->distance_to_player() < aggro) {
        npc_obj->face_player();
        npc_obj->move();
    }

}
