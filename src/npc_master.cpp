#include "../external/raylib/src/raylib.h"
#include "../include/delta_time.hpp"
#include "../include/npc.hpp"
#include "../include/player.hpp"
#include "../include/rng.hpp"
#include "../external/raylib/src/raymath.h"
#include "../include/io.hpp"
#include "../include/networking.hpp"

int npc_master::find_npc_slot_by_id(int id) {
    for (int i = 0; i < npcs.size(); i++) {
        if (npcs[i]->id == id) {
            return i;
        }
    }

    return -1;
}


serialized_npc npc_master::serialize_npc(npc *npc_c) {
    serialized_npc snpc;

    snpc.id = npc_c->id;
    snpc.pos = *npc_c->pos;
    snpc.rotation = *npc_c->rotation;
    snpc.scale = *npc_c->scale;
    snpc.size = *npc_c->size;
    snpc.stat = npc_c->stat;

    memset(snpc.type, 0, NPC_TYPE_STR_SIZE);

    size_t str_len = npc_c->template_name.size();

    if (str_len >= NPC_TYPE_STR_SIZE) {
        str_len = NPC_TYPE_STR_SIZE - 1;
    }

    memcpy(snpc.type, npc_c->template_name.c_str(), str_len);
    snpc.type[str_len] = '\0';

    if (npc_c->amsl->get_playback_status() == PLAY || npc_c->amsl->get_playback_status() == PAUSE) {
        snpc.playback_status = npc_c->amsl->get_playback_status();
        snpc.current_animation_age = npc_c->amsl->get_current_animation_age();

        std::strncpy(snpc.current_animation, npc_c->amsl->get_current_animation_name().c_str(), MAX_ANIMATE_NAME_LENGTH - 1);
        snpc.current_animation[MAX_ANIMATE_NAME_LENGTH - 1] = '\0';
    }

    return snpc;
}

int npc_master::new_npc_from_serialized_npc(serialized_npc npc_c) {


    std::string npc_type = npc_c.type;


    npc_template* nt = npc_template_manager.get_npc_template(npc_type);

    npc* new_npc = new npc;

    new_npc->assign_my_pointer((void*)new_npc);

    *new_npc->pos = npc_c.pos;

    *new_npc->size = npc_c.size;
    new_npc->stat = npc_c.stat;
    new_npc->texture_id = nt->texture_id;

    new_npc->npc_cheif_end = nt->npc_cheif_end;

    new_npc->id = npc_c.id;

    //new_npc->cache = texture_manager.grab_texture_pointer(nt->texture_id);
    new_npc->cache = nullptr;

    for (int i = 0; i < nt->animation_jsons.size(); i++) {

        std::string json_str = easy_file_ops::load_text_file(nt->animation_jsons[i]);

        new_npc->amsl->load_animation_from_json(json_str);
    }

    if (npc_c.playback_status == PLAY || npc_c.playback_status == PAUSE) {
        new_npc->amsl->play_animation(npc_c.current_animation, npc_c.looping_animation);
        new_npc->amsl->current_animation_age = npc_c.current_animation_age;
        if (npc_c.playback_status == PAUSE) {
            new_npc->amsl->pause_animation();
        }
    }

    npcs.push_back(new_npc);



    return new_npc->id;
}


void npc_master::update_npc_from_serialized_npc(serialized_npc npc_c) {

    int slot = find_npc_slot_by_id(npc_c.id);

    if (slot == -1) {
        new_npc_from_serialized_npc(npc_c);
    }

    else {
        npc* npc_u = npcs[slot];


        npc_u->last_pos = *npc_u->pos;

        *npc_u->pos = npc_c.pos;

        *npc_u->rotation = npc_c.rotation;

        *npc_u->scale = npc_c.scale;

        *npc_u->size = npc_c.size;

        npc_u->stat = npc_c.stat;

        if (npc_c.playback_status == PLAY || npc_c.playback_status == PAUSE) {
            npc_u->amsl->current_animation_age = npc_c.current_animation_age;
            npc_u->amsl->current_animation_index = npc_u->amsl->find_animation_index_by_name(npc_c.current_animation);
            npc_u->amsl->playback_status = npc_c.playback_status;
            npc_u->amsl->looping = npc_c.looping_animation;
        }
    }
}

#define SIMULATION_DISTANCE 4000


void npc_master::update_npcs() {
    if (network_manager.is_host() != true) {
        if (delta_time_master.can_game_continue()) {
            client_utls::send_npc_list_request(network_manager.get_server());
        }
    }
    else {
        for (int i = 0; i < npcs.size(); i++) {

            if (npcs[i]->distance_to_player(npcs[i]->closest_player()) > SIMULATION_DISTANCE) continue;

            if (npcs[i]->npc_data != nullptr) {
                npcs[i]->npc_data->update_colide_box();
            }
            if (npcs[i]->npc_cheif_end != nullptr  && delta_time_master.can_game_continue()) {
                npcs[i]->npc_cheif_end((void*)npcs[i]);
            }
            for (int j = 0; j < player_manager.players.size(); j++) {
                player_manager.players[j]->update_colide_box();
                npcs[i]->colide(&player_manager.players[j]->box);
            }

            auto* npc = npcs[i];

            for (size_t j = i + 1; j < npcs.size(); j++) {
                npc->colide(&npcs[j]->npc_data->box);
                npcs[j]->colide(&npc->npc_data->box);
            }

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

    new_npc->template_name = npc_type;

    new_npc->assign_my_pointer((void*)new_npc);

    *new_npc->pos = pos;

    *new_npc->size = nt->size;

    new_npc->stat = nt->stat;
    new_npc->texture_id = nt->texture_id;

    new_npc->npc_cheif_end = nt->npc_cheif_end;

    new_npc->id = random_num.get_random_int();

    *new_npc->scale = 1;

    //new_npc->cache = texture_manager.grab_texture_pointer(nt->texture_id);
    new_npc->cache = nullptr;

    for (int i = 0; i < nt->animation_jsons.size(); i++) {

        std::string json_str = easy_file_ops::load_text_file(nt->animation_jsons[i]);
        //std::cout << json_str << std::endl;

        new_npc->amsl->load_animation_from_json(json_str);
    }

    new_npc->amsl->play_animation("default", true);

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

std::string npc_master::serialize_npc_to_json(npc *npc_c) {
    serialized_npc npc_s = serialize_npc(npc_c);

    json npc_json;

    npc_json["pos"]["x"] = npc_s.pos.x;
    npc_json["pos"]["y"] = npc_s.pos.x;

    npc_json["rotation"] = npc_s.rotation;

    npc_json["scale"] = npc_s.scale;

    npc_json["size"]["x"] = npc_s.size.x;
    npc_json["size"]["y"] = npc_s.size.y;

    npc_json["stats"]["health"] = npc_s.stat.health;
    npc_json["stats"]["max_health"] = npc_s.stat.max_health;
    npc_json["stats"]["max_damage"] = npc_s.stat.max_damage;
    npc_json["stats"]["max_block"] = npc_s.stat.max_block;
    npc_json["stats"]["speed"] = npc_s.stat.speed;
    npc_json["stats"]["target_player"] = npc_s.stat.target_player;

    npc_json["id"] = npc_s.id;

    npc_json["type"] = npc_s.type;

    return npc_json.dump();
}

int npc_master::load_serialized_npc_from_json(std::string json_str) {
    json npc_json = json::parse(json_str);

    int npc_id = new_npc(npc_json["type"], {0, 0});

    npc* working_npc = npcs[find_npc_slot_by_id(npc_id)];

    if(npc_json.contains("pos")) {
        working_npc->pos->x = npc_json["pos"]["x"];
        working_npc->pos->y = npc_json["pos"]["y"];
    }

    *working_npc->rotation = npc_json.value("rotation", 0);

    *working_npc->scale = npc_json.value("scale", 1);

    working_npc->stat.health = npc_json["stats"].value("health", 1);

    working_npc->id = npc_json["id"];

    return npc_id;
}

void npc_master::clear_npcs() {
    for (int i = 0; i < npcs.size(); i++) {
        delete npcs[i];
    }

    npcs.clear();
}
