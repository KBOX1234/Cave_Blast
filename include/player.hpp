
#pragma once
#include <string>
#include <vector>

#include "render.hpp"
#include "input.hpp"
#include "texture_master.hpp"
#include "../external/raylib/src/raylib.h"

struct stat_s {
    int health;
    int hunger;
    int thirst;
};

class player_master;

class player {
    friend player_master;
    private:
        Vector2 pos;

        stat_s stats;

        int id;
        int player_texture_id;

        std::string name;


    public:

        void set_id(int id);

        void set_name(std::string name);

        int give_texture(std::string path);

        void move_player_x(float x);
        void move_player_y(float y);

        Vector2 get_pos();

        Texture2D* get_texture();

        int get_id();

        std::string get_name();

        stat_s get_stats();
};

class player_master {
    friend input;
    private:
        std::vector<player*> players;


    public:

        player* host;

        void init();

        player_master();
        ~player_master();

        int add_player(std::string name);

        std::vector<std::string> get_player_names();

        int get_player_id_by_name(std::string name);

        const player* fetch_player_data(int id);

        static void draw_player(player* pl);


};


extern player_master player_manager;
