#include "../include/player.hpp"
#include "../include/texture_master.hpp"

void player::set_id(int id2) {
    id = id2;
}

void player::set_name(std::string name2) {
    name = name2;
}

int player::give_texture(std::string path) {
    player_texture_id = texture_manager.add_texture(path, true);

    return player_texture_id;
}

void player::move_player_x(float x) {
    pos.x = pos.x + x;
}
void player::move_player_y(float y) {
    pos.y = pos.y + y;
}

Vector2 player::get_pos() {
    return pos;
}

Texture2D* player::get_texture() {
    return texture_manager.grab_texture_pointer(player_texture_id);
}

int player::get_id() {
    return id;
}

stat_s player::get_stats() {
    return stats;
}

std::string player::get_name() {
    return name;
}


int player_master::add_player(std::string name) {
    player* new_player = new player;

    new_player->set_id(players.size());

    new_player->set_name(name);


    players.push_back(new_player);

    return players.size() - 1;
}

player_master::player_master() {
    host = new player;
}

std::vector<std::string> player_master::get_player_names() {
    std::vector<std::string> names;

    for (int i = 0; i < players.size(); i++) {
        names.push_back(players[i]->get_name());
    }

    return names;
}

int player_master::get_player_id_by_name(std::string name) {
    for (int i = 0; i < players.size(); i++) {
        if (players[i]->get_name() == name) {
            return i;
        }
    }

    return -1;
}

const player *player_master::fetch_player_data(int id) {
    return (const player *)players[id];
}


player_master::~player_master() {
    for (int i = 0; i < players.size(); i++) {
        delete[] players[i];
    }

    players.clear();
}



