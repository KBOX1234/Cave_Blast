#include "../include/networking.hpp"

void net_utills::send_msg_safe(char *data, size_t s, ENetPeer *to, char channel) {
    ENetPacket *packet = enet_packet_create(data, s, ENET_PACKET_FLAG_RELIABLE);

    enet_peer_send(to, channel, packet);
}

void net_utills::send_msg_fast(char *data, size_t s, ENetPeer *to, char channel) {
    ENetPacket *packet = enet_packet_create(data, s, ENET_PACKET_FLAG_UNSEQUENCED);

    enet_peer_send(to, channel, packet);
}

size_t net_utills::get_packet_size(packet* p) {
    return sizeof(int) + sizeof(size_t) + p->size;
}

char* net_utills::convert_to_buffer(packet* p) {
    size_t total_size = get_packet_size(p);
    char* buffer = new char[total_size];

    size_t offset = 0;

    std::memcpy(buffer + offset, &p->type, sizeof(int));
    offset += sizeof(int);

    std::memcpy(buffer + offset, &p->size, sizeof(size_t));
    offset += sizeof(size_t);

    std::memcpy(buffer + offset, p->data, p->size);

    return buffer;
}