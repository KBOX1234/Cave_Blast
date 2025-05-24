#pragma once

/*
 * it is the servers job to handle data fetch requests
 * it is the clients job to fetch the data
 */

#include "world.hpp"
#include "rng.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOGDI          // Avoid GDI definitions like Rectangle()
#define NOMINMAX       // Avoid min/max macros
#define NOSERVICE
#define NOHELP
#define NOATOM
#define NOCOMM
#define NOKANJI
#define NOWH
#define NOUSER
#define NOMCX
#define NOTAPE
extern "C" __declspec(dllimport) unsigned int __stdcall timeGetTime(void);
#endif

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

        static void move_myself(float angle);

        static void send_player_list_request();

        static void fetch_player(int id);
};

class network : public net_utills {
    friend class client_utls;
private:
    bool is_server;
    ENetAddress address = {};
    ENetHost *local_instance = nullptr;
    ENetPeer *remote_instance = nullptr;

    void handle_connect(ENetEvent *event);
    void handle_disconnect(ENetEvent *event);
    void handle_request(ENetEvent *event);

    void update_server();
    void update_client();

public:
    network();
    ~network();

    bool is_host();

    void update();

};

extern network networking;