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

    offset += p->size;

    if (offset > total_size) {
        return nullptr;
    }

    return buffer;
}

packet* net_utills::convert_from_buffer(char* buffer, size_t buffer_size) {
    if (buffer_size < sizeof(int) + sizeof(size_t)) return nullptr;

    size_t offset = 0;
    int type;
    size_t size;

    std::memcpy(&type, buffer + offset, sizeof(int));
    offset += sizeof(int);

    std::memcpy(&size, buffer + offset, sizeof(size_t));
    offset += sizeof(size_t);

    if (buffer_size < offset + size) return nullptr; // Invalid/malformed

    char* data = new char[size];
    std::memcpy(data, buffer + offset, size);

    packet* p = new packet;
    p->type = type;
    p->size = size;
    p->data = data;

    return p;
}

serialized_vector_int net_utills::serializeIntVector(const std::vector<int>& vec) {
    serialized_vector_int result;
    result.size = vec.size() * sizeof(int);
    result.buffer = new char[result.size];
    std::memcpy(result.buffer, vec.data(), result.size);
    return result;
}

std::vector<int> net_utills::deserializeIntVector(const char* buffer, size_t bufferSize) {
    size_t numInts = bufferSize / sizeof(int);
    std::vector<int> vec(numInts);
    std::memcpy(vec.data(), buffer, bufferSize);
    return vec;
}