#include "networking.hpp"

void client_utls::fetch_player(int id) {
    packet* send_p = new packet;

    send_p->type = GET_PLAYER;
    send_p->size = sizeof(int);

    send_p->data = new char[sizeof(int)];

    memcpy(send_p->data, &id, sizeof(int));

    char* buffer = net_utills::convert_to_buffer(send_p);

    net_utills::send_msg_safe(buffer, net_utills::get_packet_size(send_p), networking->remote_instance, 0);
}

void client_utls::send_player_list_request() {
    packet* p = new packet;

    p->type = GET_PLAYER_LIST;
    p->size = 0;
    p->data = nullptr;

    char* buffer = net_utills::convert_to_buffer(p);

    net_utills::send_msg_safe(buffer, net_utills::get_packet_size(p), networking->remote_instance, 0);
}


void client_utls::move_myself(float angle, Vector2 pos) {
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

    net_utills::send_msg_safe(buff, net_utills::get_packet_size(p), networking->remote_instance, 0);
    //delete[] static_cast<char*>(p->data);

    //delete p;
}

void client_utls::player_creation_request(std::string name) {
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

    net_utills::send_msg_safe(buff, net_utills::get_packet_size(p), networking->remote_instance, 0);

    //delete[] static_cast<char*>(p->data);
    //delete p;
}

void client_utls::place_block(std::string name, Vector2 pos) {
    if (networking->is_server == false) {
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

        net_utills::send_msg_safe(buffer, net_utills::get_packet_size(send_p), networking->remote_instance, 0);
    }
}

void client_utls::fetch_all_players() {
    packet p;

    p.type = GET_ALL_PLAYERS;
    p.data = nullptr;
    p.size = 0;

    char* buffer = net_utills::convert_to_buffer(&p);

    net_utills::send_msg_fast(buffer, net_utills::get_packet_size(&p), networking->remote_instance, 0);

    delete[] buffer;
}
