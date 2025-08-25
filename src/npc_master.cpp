#include "../external/raylib/src/raylib.h"
#include "../include/delta_time.hpp"
#include "../include/npc.hpp"
#include "../include/player.hpp"
#include "../include/rng.hpp"
#include "../external/raylib/src/raymath.h"
#include "../include/networking.hpp"

void npc_master::update_npcs() {
    if (network_manager.is_host() != true) {

    }
    for (int i = 0; i < npcs.size(); i++) {
        if (npcs[i]->npc_data != nullptr) {
            npcs[i]->npc_data->update_colide_box();
        }
        if (npcs[i]->npc_cheif_end != nullptr  && delta_time_master.can_game_continue()) {
            npcs[i]->npc_cheif_end((void*)npcs[i]);
        }

    }
}

void npc_master::draw_npcs() {
    for (int i = 0; i < npcs.size(); i++) {
        npcs[i]->draw();
    }
}

int npc_master::new_npc(std::string npc_type, Vector2 pos) {

    if (network_manager.is_host() != true) {
        std::cout << "(NPC_MANAGER) Cannot spawn entity as client!\n";
        return -1;
    }

    npc_template* nt = npc_template_manager.get_npc_template(npc_type);

    npc* new_npc = new npc;

    new_npc->assign_my_pointer((void*)new_npc);

    *new_npc->pos = pos;

    *new_npc->size = nt->size;
    new_npc->stat = nt->stat;
    new_npc->texture_id = nt->texture_id;

    new_npc->npc_cheif_end = nt->npc_cheif_end;

    new_npc->id = random_num.get_random_int();

    //new_npc->cache = texture_manager.grab_texture_pointer(nt->texture_id);
    new_npc->cache = nullptr;
    npcs.push_back(new_npc);

    

    return new_npc->id;

}

bool npc_master::remove_npc(int id) {
    if (network_manager.is_host() != true) {
        std::cout << "(NPC_MANAGER): Cannot remove npc as client!\n";
    }
    for (int i = 0; i < npcs.size(); i++) {
        if (npcs[i]->id == id) {
            npcs.erase(npcs.begin() + i);

            return true;
        }
    }

    return false;
}

Vector2 npc::get_pos() {
    return *pos;
}

player* npc::closest_player() {

    player* p;
    float fp = 0xffffffff;

    for (int i = 0; i < player_manager.players.size(); i++) {
        float p2 = Vector2Distance(*pos, *player_manager.players[i]->pos);

        if (p2 < fp) {
            fp = p2;

            p = player_manager.players[i];

        }
    }

    return p;
}

void npc::face_player() {
    Vector2 object = *pos;
    player* p = closest_player();

    Vector2 target = p->get_pos();


    float dx = target.x - object.x;
    float dy = target.y - object.y;

    float angleRad = std::atan2(dy, dx);

    float angleDeg = angleRad * 180.0f / M_PI;

    *rotation = angleDeg;
}
