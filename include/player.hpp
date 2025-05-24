
#pragma once
#include <string>
#include <vector>

#include "render.hpp"
#include "input.hpp"
#include "texture_master.hpp"
#include "../external/raylib/src/raylib.h"
#include "imgui_window.hpp"
#include "networking.hpp"

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

    char name[MAX_NAME_LENGTH];
};

class player_master;

class player {
    friend class player_master;
    private:
        Vector2 pos;

        stat_s stats;

        int id;
        int player_texture_id;

        std::string name;


    public:

        player();
        //~player();


        void set_id(int id);

        void set_pos(Vector2 pos);

        void set_name(std::string name);

        int give_texture(std::string path);

        void move_player_x(float x);
        void move_player_y(float y);

        Vector2 get_pos();

        Texture2D* get_texture();

        int get_id();

        std::string get_name();

        stat_s get_stats();

        serialized_player serialize();

        void set_stats(stat_s st);
};

class player_master {
    friend class imgui_win;
    friend class input;
    friend class network;
    friend class render;
    private:
        std::vector<player*> players;

        int add_player_from_serialised_player(serialized_player* spl);


    public:

        int default_texture_id;

        int host_id;

        void init();

        player_master();
        ~player_master();

        player* get_host();

        int add_player(std::string name);

        std::vector<std::string> get_player_names();

        int get_player_id_by_name(std::string name);

        player* fetch_player_data(int id);

        void draw_player(player* pl);

        bool does_player_exist(int id);


};


extern player_master player_manager;
