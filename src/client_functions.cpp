#include "networking.hpp"
#include "world.hpp"
#include "rng.hpp"
#include "httplib.h"
#include "player.hpp"

void client_utls::fetch_player(int id, ENetPeer* srv_r) {
    packet* send_p = new packet;

    send_p->type = GET_PLAYER;
    send_p->size = sizeof(int);

    send_p->data = new char[sizeof(int)];

    memcpy(send_p->data, &id, sizeof(int));

    char* buffer = net_utills::convert_to_buffer(send_p);

    net_utills::send_msg_safe(buffer, net_utills::get_packet_size(send_p), srv_r, 0);
}

void client_utls::send_player_list_request(ENetPeer* srv_r) {
    packet* p = new packet;

    p->type = GET_PLAYER_LIST;
    p->size = 0;
    p->data = nullptr;

    char* buffer = net_utills::convert_to_buffer(p);

    net_utills::send_msg_safe(buffer, net_utills::get_packet_size(p), srv_r, 0);
}


void client_utls::move_myself(float angle, Vector2 pos, ENetPeer* srv_r) {
    packet* p = new packet;

    p->type = MOVE;
    p->size = sizeof(Vector2) + sizeof(float);

    p->data = new char[p->size];

    memcpy(p->data, &pos, sizeof(Vector2));
    memcpy(p->data + sizeof(Vector2), &pos, sizeof(float));

    char* buff = net_utills::convert_to_buffer(p);

    if (buff == nullptr) {
        std::cout << "\nissue packing packet\n";
        return;
    }

    net_utills::send_msg_safe(buff, net_utills::get_packet_size(p), srv_r, 0);
    //delete[] static_cast<char*>(p->data);

    //delete p;
}

void client_utls::player_creation_request(std::string name, ENetPeer* srv_r) {
    packet* p = new packet;

    p->type = CREATE_PLAYER;

    const char* new_str = name.c_str();
    p->size = strlen(new_str) + 1;

    p->data = new char[p->size];


    printf("requesting new player: %s", new_str);
    memcpy(p->data, new_str, p->size);

    char* buff = net_utills::convert_to_buffer(p);

    if (buff == nullptr) {
        std::cout << "\nissue packing packet\n";
        return;
    }

    net_utills::send_msg_safe(buff, net_utills::get_packet_size(p), srv_r, 0);

    //delete[] static_cast<char*>(p->data);
    //delete p;
}

void client_utls::place_block(std::string name, Vector2 pos, ENetPeer* srv_r) {
    if (network_manager.is_server == false) {
        size_t data_size = strlen(name.c_str()) + 1;

        data_size = data_size + sizeof(Vector2);

        packet* send_p = new packet;

        send_p->type = SET_BLOCK;

        send_p->size = data_size;

        char* data_b = new char[data_size];

        size_t point = sizeof(Vector2);

        memcpy(data_b, &pos, point);

        memcpy(data_b + point, name.c_str(), strlen(name.c_str()) + 1);

        send_p->data = data_b;

        char* buffer = net_utills::convert_to_buffer(send_p);

        net_utills::send_msg_safe(buffer, net_utills::get_packet_size(send_p), srv_r, 0);
    }
}

void client_utls::fetch_all_players(ENetPeer* srv_r) {
    packet p;

    p.type = GET_ALL_PLAYERS;
    p.data = nullptr;
    p.size = 0;

    char* buffer = net_utills::convert_to_buffer(&p);

    net_utills::send_msg_fast(buffer, net_utills::get_packet_size(&p), srv_r, 0);

    delete[] buffer;
}

void client::fetch_chunk(Vector2 pos) {
    async_chunk_fetch_on = true;
    json sendJ;

    sendJ["x"] = pos.x;
    sendJ["y"] = pos.y;

    auto res = cli->Post("/chunk", sendJ.dump(), "application/json");

    if (res && res->status == 200) {

        //std::cout << "Response:\n" << res->body << std::endl;
        json new_c = json::parse(res->body);
        world.new_chunk_from_json(new_c);

    }
    async_chunk_fetch_on = false;
}

void client_utls::break_block(ENetPeer* srv_r, Vector2 pos, const std::string& item) {
    packet p;

    p.type = BREAK_BLOCK;

    size_t total_size = sizeof(Vector2) + item.length() + 1;
    p.size = total_size;

    p.data = new char[total_size];

    memcpy(p.data, &pos, sizeof(Vector2));
    memcpy(p.data + sizeof(Vector2), item.c_str(), item.length() + 1);

    char* buffer = net_utills::convert_to_buffer(&p);

    net_utills::send_msg_safe(buffer, net_utills::get_packet_size(&p), srv_r, 0);

    delete[] buffer;
    delete[] p.data;
}

void client_utls::craft_item(int craft_index) {
    packet p;

    p.type = CRAFT_ITEM;

    p.size = sizeof(int);

    p.data = (char*)&craft_index;

    char* buffer = net_utills::convert_to_buffer(&p);

    net_utills::send_msg_safe(buffer, net_utills::get_packet_size(&p), network_manager.get_server(), 0);

    delete[] buffer;

}

void client_utls::convert_item(item *itm, item *table) {
    packet p;

    int itm_str_len = itm->name.size() + 1;

    int table_str_len = table->name.size() + 1;

    p.size = (sizeof(int) * 2) + table_str_len + itm_str_len;

    char* buffer = new char[p.size];

    size_t pointer;

    memcpy(buffer, &itm_str_len, sizeof(int));

    pointer = pointer + sizeof(int);

    memcpy(buffer + pointer, &table_str_len, sizeof(int));

    pointer = pointer + sizeof(int);

    memcpy(buffer + pointer, itm->name.c_str(), itm_str_len);

    pointer = pointer + itm_str_len;

    memcpy(buffer + pointer, table->name.c_str(), table_str_len);

    p.data = buffer;

    p.type = CONVERT_ITEM;

    char* other_b = net_utills::convert_to_buffer(&p);

    net_utills::send_msg_safe(other_b, net_utills::get_packet_size(&p), network_manager.get_server(), 0);

    delete[] other_b;
    delete[] p.data;
}
