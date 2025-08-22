#include "../include/npc.hpp"
#include "../include/rng.hpp"

void npc_master::update_npcs() {
    for (int i = 0; i < npcs.size(); i++) {
        if (npcs[i]->npc_data != nullptr) {
            npcs[i]->npc_data->update_colide_box();
        }
        if (npcs[i]->npc_cheif_end != nullptr) {
            npcs[i]->npc_cheif_end((void*)npcs[i]->npc_data);
        }

    }
}

void npc_master::draw_npcs() {
    for (int i = 0; i < npcs.size(); i++) {
        npcs[i]->draw();
    }
}

int npc_master::new_npc(std::string npc_type, Vector2 pos) {
    npc_template* nt = npc_template_manager.get_npc_template(npc_type);

    npc* new_npc = new npc;

    new_npc->assign_my_pointer((void*)new_npc);

    *new_npc->pos = pos;

    *new_npc->size = nt->size;
    new_npc->stat = nt->stat;
    new_npc->texture_id = nt->texture_id;

    new_npc->npc_cheif_end = nt->npc_cheif_end;

    new_npc->id = random_num.get_random_int();

    npcs.push_back(new_npc);

    return new_npc->id;

}

bool npc_master::remove_npc(int id) {
    for (int i = 0; i < npcs.size(); i++) {
        if (npcs[i]->id == id) {
            npcs.erase(npcs.begin() + i);

            return true;
        }
    }

    return false;
}



