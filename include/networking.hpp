#pragma once
#include "world.hpp"

#define PORT 8089

#include <enet/enet.h>

struct __attribute__((packed)) packet {
    int type;
    size_t size;
    void* data;
};

class net_utills {
public:
    static void send_msg_safe(char* data, size_t s, ENetPeer *to, char channel);

    static void send_msg_fast(char* data, size_t s, ENetPeer *to, char channel);

    static size_t get_packet_size(packet* p);

    static char* convert_to_buffer(packet* p);

};

class network : public net_utills {
private:
    bool is_server;
    ENetAddress address = {};
    ENetHost *local_instance = nullptr;
    ENetPeer *remote_instance = nullptr;

public:
    network();
    ~network();

    void sync_world_data();

};

extern network networking;