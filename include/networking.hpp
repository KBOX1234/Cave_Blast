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
#define NOMINMAX 1
#define NOUSER
#define NOCOMM
#define NOIME
#define NOSERVICE
#define NOMCX
#endif

#include <enet/enet.h>
#include <thread>
#include <iostream>
#include "httplib.h"
#include "raylib.h"
#include "inventory.hpp"

#define NOTHING 0
#define MOVE 1
#define GET_CHUNK 2
#define SET_BLOCK 3
#define CREATE_PLAYER 4
#define GET_PLAYER_LIST 5
#define GET_PLAYER 6
#define DISCONNECT_PLAYER 7
#define RE_CALIBRATE 8
#define GET_ALL_PLAYERS 9
#define BREAK_BLOCK 10
#define GIVE_BLOCK 11

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

        //requests
        static void player_creation_request(std::string name, ENetPeer* srv_r);

        static void move_myself(float angle, Vector2 pos, ENetPeer* srv_r);

        static void send_player_list_request(ENetPeer* srv_r);

        static void fetch_player(int id, ENetPeer* srv_r);

        static void place_block(std::string name, Vector2 pos, ENetPeer* srv_r);

        static void fetch_all_players(ENetPeer* srv_r);

        static void break_block(ENetPeer* srv_r, Vector2 pos, const std::string& item);



};

class server_utls {
    public:
        static void handle_player_creation(ENetEvent* event, packet* p);

        static void handle_move_packet(ENetEvent* event, packet* p);

        static void send_player_list(ENetEvent* event, packet* p);

        static void send_player_data(ENetEvent* event, packet* p);

        static void handle_player_block_placement(ENetEvent* event, packet* p);

        static void handle_all_player_fetch(ENetEvent* event);

        static void send_p_connection_loss(ENetEvent *event);

        static void handle_player_break_block(ENetEvent* event, packet* p);

        static void give_player_item(ENetPeer* peer, std::string item, char count);

};

struct block_change {
    std::string blk_name;

    Vector2 pos;
};

class network;

class server : public server_utls{
    
    friend class client_utls;
    friend class server_utls;
    friend class world_class;
    friend class network;

    private:

        std::string ip_addr;
        int port;

        //enet host object
        ENetHost* host_server = nullptr;


        //chunk sync server
        httplib::Server svr;

        //clients vector
        std::vector<ENetPeer*> clients;

        //block changes vector
        std::vector<block_change> blk_change;

        ENetAddress address = {};

        void handle_connect(ENetEvent *event);

        void handle_disconnect(ENetEvent *event);

        void handle_request(ENetEvent *event);

        void send_block_changes(ENetPeer *to);

        void broadcast_block_changes();

        void broadcast_disconnect(ENetEvent *event);

        bool async_chunk_fetch_on = false;

    public:

        void start_api();

        void start_server(const std::string ip, int port);

        ~server();

        void add_block_change(block_change blk_chng);

        void update();

        ENetPeer* get_peer_by_player_id(int id);


};

class client : public client_utls{
    friend class client_utls;
    friend class server_utls;
    friend class world_class;
    friend class network;

    private:

        bool async_chunk_fetch_on = false;

        int port;

        std::string ip_addr;

        ENetAddress address = {};

        ENetPeer* remote_server = nullptr;

        ENetHost* myself = nullptr;

        std::unique_ptr<httplib::Client> cli;

    public:

        void fetch_chunk(Vector2 pos);

        void start_client(std::string ip, int port);

        void update();

        void handle_player_list_response(ENetEvent* event, packet* p);

        void handle_player_get_response(ENetEvent* event, packet* p);

        void handle_player_creation_response(ENetEvent* event, packet* p);

        void handle_block_updates(ENetEvent* event, packet* p);

        void handle_other_player_disconnect(ENetEvent* event, packet* p);

        void handle_recalibrate_request_from_server(ENetEvent* event, packet* p);

        void handle_big_ahh_player_packet_with_all_players_from_the_server_for_which_data_is_comming_from(ENetEvent* event, packet* p);

        void handel_get_item_from_server(ENetEvent* event, packet* p);
};

class network : public net_utills {
    friend class client_utls;
    friend class server_utls;
    friend class world_class;
    private:
        int port;
        std::string ip_addr;

        bool is_server;


    public:

        server server_obj;

        client client_obj;

        void start(std::string ip, int port, bool is_server);

        bool is_host();

        ENetPeer* get_server();

        void update();

};

extern network network_manager;