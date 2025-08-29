
#include "../../include/npcs.hpp"
#include "../../external/raylib/src/raymath.h"

void npc_scripts::rattle_bones(void *obj) {
    npc* npc_obj = (npc*)obj;
    const float aggro = 128;

    if (npc_obj->data_bank.does_object_exist("is_loaded")) {
        npc_obj->data_bank.add_entry(true, "is_loaded");

    }

    player* pp = npc_obj->closest_player();

    float dist = Vector2Distance(npc_obj->get_pos(), pp->get_pos());

    if (dist < aggro) {
        if (!npc_obj->data_bank.does_object_exist("walking")) {
            npc_obj->data_bank.add_entry(true, "walking");
            npc_obj->npc_data->amsl.play_animation("walk", true);
        }
        npc_obj->face_player(pp);
        npc_obj->move();
    }
    else {
        if (npc_obj->data_bank.does_object_exist("walking")) {
            npc_obj->data_bank.free_object("walking");
            npc_obj->npc_data->amsl.play_animation("default");
        }
    }

}
