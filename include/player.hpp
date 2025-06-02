
#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <cmath>

#include "render.hpp"
#include "input.hpp"
#include "texture_master.hpp"
#include "../external/raylib/src/raylib.h"
#include "imgui_window.hpp"
#include "networking.hpp"
#include "raymath.h"
#include "delta_time.hpp"
#include "inventory.hpp"

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

class player {
    friend class player_master;
    friend class render;
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

        inventory inv;


    public:

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

};

class player_master {
    friend class imgui_win;
    friend class input;
    friend class network;
    friend class server;
    friend class client;
    friend class render;
    friend class server_utls;
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


};


extern player_master player_manager;
