#include "../include/networking.hpp"

void net_utills::send_msg_safe(char *data, size_t s, ENetPeer *to, char channel) {
    ENetPacket *packet = enet_packet_create(data, s, ENET_PACKET_FLAG_RELIABLE);

    enet_peer_send(to, channel, packet);
}

void net_utills::send_msg_fast(char *data, size_t s, ENetPeer *to, char channel) {
    ENetPacket *packet = enet_packet_create(data, s, ENET_PACKET_FLAG_UNSEQUENCED);

    enet_peer_send(to, channel, packet);
}

size_t net_utills::get_packet_size(packet *p) {
    size_t type_val_size = sizeof(int);
    size_t total_size = type_val_size + p->size;

    return total_size;
}

char* net_utills::convert_to_buffer(packet* p) {
    int pointer = 0;

    char* buffer = new char[get_packet_size(p)];

    memcpy(buffer, &p->type,  sizeof(int));
    pointer = pointer + sizeof(int);
    memcpy(&buffer[pointer], &p->size, sizeof(size_t));
    pointer = pointer + sizeof(size_t);

    memcpy(&buffer[pointer], p->data, p->size);

    return buffer;

}