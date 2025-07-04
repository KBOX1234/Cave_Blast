
#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include "raylib.h"

#include "inventory.hpp"
#include "sphysics.h"
#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 64

struct stat_s {
    int health;
    int hunger;
    int thirst;
};

#define MAX_NAME_LENGTH 40

struct serialized_player {
    Vector2 pos;

    stat_s stats;

    int id;

    float angle;

    char name[MAX_NAME_LENGTH];
};

class player_master;
class colide;
class server_utls;

class player {
    friend class player_master;
    friend class render;
    friend class colide;
    friend class server_utls;
    private:
        Vector2 pos;

        stat_s stats;

        int id;
        int player_texture_id;

        int speed = 3;

        float rotation = 0;

        std::string name;

        double last_move = (double)std::chrono::duration_cast<std::chrono::milliseconds>(
std::chrono::high_resolution_clock::now().time_since_epoch()
).count();
        Vector2 last_pos;

        Vector2 interpolation;

        colideBox box;

        bool is_valid_move_2(Vector2 pos2);

        Vector2 get_move_target();

        int light_index;

        char current_table = 0;


    public:

        int get_light_index();


        void update_colide_box();

        inventory inv;

        player();
        //~player();

        int64_t last_move_tsmp();

        void update_time_stamp();

        bool is_valid_move(Vector2 pos2);


        void set_id(int id);

        void set_pos(Vector2 pos2);

        void set_name(std::string name);

        int give_texture(std::string path);

        void move_player();

        void move_player_back();

        Vector2 get_pos();

        Vector2 get_block_pos();

        Texture2D* get_texture();

        int get_id();

        std::string get_name();

        stat_s get_stats();

        serialized_player serialize();

        void set_stats(stat_s st);

        void increase_angle(float amount, bool no_delta = false);

        void decrease(float amount);

        void zero_rotation(bool no_delta = false);

        float get_rotation();

        Vector2 get_interpos();

        bool break_block(Vector2 pos);

        bool place_block(Vector2 pos);

};

class colide;

class player_master {
    friend class imgui_win;
    friend class input;
    friend class network;
    friend class server;
    friend class client;
    friend class render;
    friend class server_utls;

    friend class colide;
    private:
        std::vector<player*> players;

        int add_player_from_serialised_player(serialized_player* spl);

        int re_sync_timer = 0;


    public:

        inventory_ui inv_ui;

        int default_texture_id;

        player* myself;

        int host_id;

        void init();

        player_master();
        ~player_master();

        player* get_host();

        int add_player(std::string name);

        int remove_player(int id);

        std::vector<std::string> get_player_names();

        int get_player_id_by_name(std::string name);

        player* fetch_player_data(int id);

        void draw_player(player* pl);

        bool does_player_exist(int id);

        void update_predicted_player();

        void give_player_item(item* i, int count, int player_id);


};


extern player_master player_manager;
