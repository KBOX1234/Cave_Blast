#include "networking.hpp"
#include "world.hpp"
#include "rng.hpp"
#include "httplib.h"
#include "inventory.hpp"
#include "player.hpp"
#include "item_master.hpp"
#include "lighting.hpp"
#include "crafting.hpp"
#include "item_convert.hpp"

void server::add_block_change(block_change blk_chng) {

    if(blk_change.size() == 0 || blk_change.size() == 1){
        blk_change.push_back(blk_chng);

        return;
    }

    if(blk_change[blk_change.size() - 2].blk_name != blk_chng.blk_name && blk_change[blk_change.size() - 2].pos.x != blk_chng.pos.x && blk_change[blk_change.size() - 2].pos.y != blk_chng.pos.y){
        blk_change.push_back(blk_chng);
    }

}

void server::send_block_changes(ENetPeer *to) {
    for (int i = 0; i < blk_change.size(); i++) {
        packet p;

        p.type = SET_BLOCK;

        size_t total_d_size = strlen(blk_change[i].blk_name.c_str()) + 1 + sizeof(Vector2);

        char* data_b = new char[total_d_size];

        memcpy(data_b, &blk_change[i].pos, sizeof(Vector2));
        memcpy(data_b + sizeof(Vector2), blk_change[i].blk_name.c_str(), strlen(blk_change[i].blk_name.c_str()) + 1);

        p.size = total_d_size;
        p.data = data_b;

        char* buffer = net_utills::convert_to_buffer(&p);

        net_utills::send_msg_safe(buffer,  net_utills::get_packet_size(&p), to, 0);

    }

}

void server_utls::handle_player_creation(ENetEvent *event, packet* p) {
    std::string new_name(p->data, p->size);
    int p_id = player_manager.add_player(new_name);

    auto* p_id_ptr = new int(p_id);
    event->peer->data = p_id_ptr;

    player* plr = player_manager.fetch_player_data(p_id);

    serialized_player splr = plr->serialize();

    packet send_p;
    send_p.type = CREATE_PLAYER;
    send_p.size = sizeof(serialized_player);
    send_p.data = (char*)&splr;

    char* buffer = net_utills::convert_to_buffer(&send_p);
    net_utills::send_msg_safe(buffer, net_utills::get_packet_size(&send_p), event->peer, 0);

    delete[] buffer;
}

void server_utls::handle_move_packet(ENetEvent *event, packet *p) {
    int* id_ptr = static_cast<int*>(event->peer->data);
    if (id_ptr && p->size >= sizeof(float) + sizeof(Vector2)) {
        int id = *id_ptr;
        Vector2 pos;
        float angle;
        std::memcpy(&pos, p->data, sizeof(Vector2));
        memcpy(&angle, p->data + sizeof(Vector2), sizeof(float));

        player_manager.players[id]->zero_rotation();
        player_manager.players[id]->increase_angle(angle);

        packet back_p;

        back_p.type = RE_CALIBRATE;
        back_p.size = sizeof(Vector2);

        //does not want to work so for now we will take client coords at face value
        //if (player_manager.players[id]->is_valid_move(pos) == true) {
        if (true) {
            player_manager.players[id]->set_pos(pos);
            light_manager.update_light_pos(player_manager.players[id]->light_index, pos);
            player_manager.players[id]->update_time_stamp();
            back_p.data = (char*)&pos;


        }
        else {
            pos = player_manager.players[id]->get_pos();
            back_p.data = (char*)&pos;

            std::cout << "player: " << std::to_string(*(int*)event->peer->data) << " moved wrongly\n";
        }

        char* buffer = net_utills::convert_to_buffer(&back_p);

        net_utills::send_msg_safe(buffer, net_utills::get_packet_size(&back_p), event->peer, 0);

        delete[] buffer;


        //std::cout << "player \"" << id << "\" moved to " << std::to_string(pos.x) << ", " << std::to_string(pos.y) << "." << std::endl;
    } else {
        std::cerr << "[network] MOVE packet malformed or peer ID missing" << std::endl;
    }
}

void server_utls::send_player_list(ENetEvent *event, packet *p) {
    packet send_p;

    send_p.type = GET_PLAYER_LIST;

    std::vector<int> ids;

    ids.push_back(player_manager.get_host()->get_id());

    for (int i = 0; i < player_manager.players.size(); i++) {
        ids.push_back(player_manager.players[i]->get_id());
    }

    serialized_vector_int datad = net_utills::serializeIntVector(ids);

    send_p.size = datad.size;
    send_p.data = datad.buffer;

    char* buffer = net_utills::convert_to_buffer(&send_p);

    net_utills::send_msg_safe(buffer, net_utills::get_packet_size(&send_p), event->peer, 0);

    delete[] datad.buffer;

    delete[] buffer;
}

void server_utls::send_player_data(ENetEvent *event, packet *p) {
    packet send_p;

    send_p.type = GET_PLAYER;

    int* id = (int*)p->data;

    if (player_manager.fetch_player_data(*id) == nullptr) {
        std::cout << "(SERVER): ID: " << std::to_string(*id) << "does not exist\n";
    }

    else {
        serialized_player srp = player_manager.fetch_player_data(*id)->serialize();

        send_p.size = sizeof(serialized_player);

        send_p.data = (char*)&srp;

        char* buffer = net_utills::convert_to_buffer(&send_p);

        net_utills::send_msg_fast(buffer, net_utills::get_packet_size(&send_p), event->peer, 0);

        delete[] buffer;
    }
}

void server_utls::handle_player_block_placement(ENetEvent *event, packet *p) {

    player* pl = player_manager.fetch_player_data(*(int*)event->peer->data);

    Vector2 b_p_pos;

    memcpy(&b_p_pos, p->data, sizeof(Vector2));

    char* name_b = new char[p->size - sizeof(Vector2)];

    memcpy(name_b, p->data + sizeof(Vector2), p->size - sizeof(Vector2));

    block blk;

    blk.state = 0;
    blk.attr = item_manager.fetch_item(name_b)->block_type_ptr;

    if(pl->inv.does_have_item(item_manager.fetch_item(name_b), 1) == false) return;

    world.place_block(b_p_pos, blk);
}

void server_utls::handle_all_player_fetch(ENetEvent *event) {
    packet send_p;

    send_p.type = GET_ALL_PLAYERS;

    serialized_player serialized_players[player_manager.players.size()];

    size_t data_size = sizeof(serialized_player) * player_manager.players.size();

    size_t inc = 0;

    char* data_b = new char[data_size];


    for (int i = 0; i < player_manager.players.size(); i++) {
        serialized_players[i].angle = player_manager.players[i]->get_rotation();
        serialized_players[i].id = player_manager.players[i]->get_id();

        serialized_players[i].pos = player_manager.players[i]->get_pos();
        serialized_players[i].stats = player_manager.players[i]->get_stats();

        const char* pname = player_manager.players[i]->get_name().c_str();

        size_t name_length = strlen(pname) + 1;

        if (name_length >= MAX_NAME_LENGTH) {
            memcpy(serialized_players[i].name, pname, MAX_NAME_LENGTH -1);
            serialized_players[i].name[MAX_NAME_LENGTH -1] = '\0';
        }
        else {
            memcpy(serialized_players[i].name, pname, name_length);
        }

        memcpy(data_b + inc, &serialized_players[i], sizeof(serialized_player));

        inc = inc + sizeof(serialized_player);

    }

    send_p.size = data_size;

    send_p.data = data_b;

    char* buffer = net_utills::convert_to_buffer(&send_p);

    net_utills::send_msg_safe(buffer, net_utills::get_packet_size(&send_p), event->peer, 0);

    delete[] buffer;
    delete[] data_b;
}

void server_utls::handle_player_break_block(ENetEvent* event, packet* p){
    Vector2 block_pos;

    memcpy(&block_pos, p->data, sizeof(Vector2));

    char tmp_str[p->size - sizeof(Vector2)];

    memcpy(tmp_str, p->data + sizeof(Vector2), p->size - sizeof(Vector2));

    std::string tool_name(tmp_str);

    item* itm = item_manager.fetch_item(tool_name);

    if(itm == nullptr) return; 

    if(itm->is_weapon != true){
        return;
    }

    if(
        itm->strength < item_manager.fetch_item_by_id(world.get_block(block_pos)->attr->item_id)->strength
        && item_manager.fetch_item_by_id(world.get_block(block_pos)->attr->item_id)->strength > 0
    ) return;

    player* pl = player_manager.fetch_player_data(*(int*)event->peer->data);

    if(pl == nullptr) return;

    pl->inv.give_item(item_manager.fetch_item_by_id(world.get_block(block_pos)->attr->item_id), 1);

    block blk;

    blk.state = 0;
    blk.attr = item_manager.fetch_item("air")->block_type_ptr;

    world.place_block(block_pos, blk);
}

void server_utls::give_player_item(ENetPeer* peer, std::string item, char count){
    packet p;

    p.type = GIVE_BLOCK;
    p.size = sizeof(char) + strlen(item.c_str()) + 1;

    p.data = (char*)malloc(p.size);

    memcpy(p.data, &count, sizeof(char));

    memcpy(p.data + sizeof(char), item.c_str(), strlen(item.c_str()) + 1);

    char* buffer = net_utills::convert_to_buffer(&p);

    if(peer == nullptr){
        //std::cout << "[cannot give player item\nerror: peer is NULL]";
        return;
    }

    net_utills::send_msg_safe(buffer, net_utills::get_packet_size(&p), peer, 0);

    free(p.data);
}

void server_utls::handle_craft_request(ENetEvent *event, packet *p) {
    player* pl = player_manager.fetch_player_data(*(int*)event->peer->data);

    if (pl == nullptr) return;

    int ci;

    memcpy(&ci, p->data, sizeof(int));

    crafting_manager.craft_item(ci, pl);
}

void server_utls::handle_convert_request(ENetEvent *event, packet *p) {
    player* pl = player_manager.fetch_player_data(*(int*)event->peer->data);

    if (pl == nullptr) return;

    int item_name_length;

    int table_name_length;

    size_t pointer;

    memcpy(&item_name_length, p->data, sizeof(int));

    pointer = pointer + sizeof(int);

    memcpy(&table_name_length, p->data + pointer, sizeof(int));

    pointer = pointer + sizeof(int);

    char item_name[item_name_length];

    memcpy(item_name, p->data + pointer, item_name_length);

    pointer = pointer + item_name_length;

    char table_name[table_name_length];

    memcpy(table_name, p->data + pointer, table_name_length);

    item* itm = item_manager.fetch_item(item_name);

    item* table_i = item_manager.fetch_item(table_name);

    block blk;
    blk.attr = table_i->block_type_ptr;
    blk.state = 0;

    item_convert_manager.convert_item(itm, blk, pl);


}
