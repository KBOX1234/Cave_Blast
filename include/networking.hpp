#pragma once


/*
 * it is the servers job to handle data fetch requests
 * it is the clients job to fetch the data
 */

#include "world.hpp"
#include "rng.hpp"

#define PORT 8089

#define NOTHING 0
#define MOVE 1
#define GET_CHUNK 2
#define CREATE_PLAYER 3
#define GET_PLAYER_LIST 4
#define GET_PLAYER 5

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

    void handle_connect(ENetEvent *event);
    void handle_disconnect(ENetEvent *event);
    void handle_request(ENetEvent *event);

    void update_server();

public:
    network();
    ~network();

    void update();

};

extern network networking;