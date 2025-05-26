#pragma once

/*
 * it is the servers job to handle data fetch requests
 * it is the clients job to fetch the data
 */

//trying to stop windows.h from poluting the namespace
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOGDI             // Prevent inclusion of GDI (Graphics Device Interface), including Rectangle()
#define NOSYSMETRICS
#define NOMINMAX
#define NOUSER
#define NOCOMM
#define NOIME
#define NOSERVICE
#define NOMCX
#endif

#include <enet/enet.h>

#define PORT 8089
#define API_PORT 8080

#define NOTHING 0
#define MOVE 1
#define GET_CHUNK 2
#define SET_BLOCK 3
#define CREATE_PLAYER 4
#define GET_PLAYER_LIST 5
#define GET_PLAYER 6
#define DISCONNECT_PLAYER 7
#define RE_CALIBRATE 8

#include "world.hpp"
#include "rng.hpp"
#include "httplib.h"
#include <thread>

struct __attribute__((packed)) packet {
    int type;
    size_t size;
    char* data;
};

struct serialized_vector_int {
    char* buffer;
    size_t size;
};

class net_utills {
public:
    static void send_msg_safe(char* data, size_t s, ENetPeer *to, char channel);

    static void send_msg_fast(char* data, size_t s, ENetPeer *to, char channel);

    static size_t get_packet_size(packet* p);

    static char* convert_to_buffer(packet* p);

    static packet* convert_from_buffer(char* buffer, size_t buffer_size);

    static serialized_vector_int serializeIntVector(const std::vector<int>& vec);

    static std::vector<int> deserializeIntVector(const char* buffer, size_t bufferSize);

};

class client_utls {
    public:

        static void player_creation_request(std::string name);

        static void move_myself(float angle, Vector2 pos);

        static void send_player_list_request();

        static void fetch_player(int id);

        static void place_block(std::string name, Vector2 pos);

};

struct block_change {
    std::string blk_name;

    Vector2 pos;
};

class network : public net_utills {
    friend class client_utls;
    friend class world_class;
private:

    bool async_chunk_fetch_on = false;

    void fetch_chunk(Vector2 pos);

    bool is_server;
    ENetAddress address = {};
    ENetHost *local_instance = nullptr;
    ENetPeer *remote_instance = nullptr;
    std::unique_ptr<httplib::Client> cli;
    httplib::Server svr;
    std::vector<ENetPeer*> clients;

    std::vector<block_change> blk_change;

    void send_block_changes(ENetPeer* to);

    void handle_connect(ENetEvent *event);
    void handle_disconnect(ENetEvent *event);
    void handle_request(ENetEvent *event);
    void send_p_connection_loss(ENetEvent* event);

    void update_server();
    void update_client();

    void start_api();

public:
    network();
    ~network();

    bool is_host();
    void add_block_change(block_change blk_chng);

    void update();

};

extern network networking;