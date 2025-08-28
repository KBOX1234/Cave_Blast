#include "../include/npcs.hpp"

void test_npc_setup(npc* obj) {

}

void npc_scripts::test(void* obj){
    npc* npc_obj = (npc*)obj;
    const float aggro = 64;

    if (npc_obj->data_bank.does_object_exist("is_loaded")) {
        npc_obj->data_bank.add_entry(true, "is_loaded");

        test_npc_setup(npc_obj);
    }

    if (npc_obj->distance_to_player() < aggro) {
        
    }
}
