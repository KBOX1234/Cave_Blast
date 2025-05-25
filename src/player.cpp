#include "../include/player.hpp"
#include "../include/texture_master.hpp"
#include "../include/world.hpp"

player::player() {
    pos = {0, 0};
}


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

void player::decrease(float amount) {
    rotation = rotation - amount;
}

void player::increase_angle(float amount) {
    rotation = rotation + amount;
}

void player::zero_rotation() {
    rotation = 0;
}

float player::get_rotation() {
    return rotation;
}



void player::move_player() {
    Vector2 d;

    float rad = rotation * (M_PI / 180.0f);

    d.x = std::cos(rad) * speed;
    d.y = std::sin(rad) * speed;

    pos.x += d.x;
    pos.y += d.y;
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

    std::cout << "Player " << name << " added" << std::endl;

    return players.size() - 1;
}

player_master::player_master() {
    host_id = random_num.get_random_int();
}

player *player_master::get_host() {
    for (int i = 0; i < players.size(); i++) {
        if (players[i]->get_id() == host_id) {
            return players[i];
        }
    }

    return nullptr;
}


void player_master::init() {


    default_texture_id = texture_manager.add_texture("reasource/gfx/other/player.png", true);
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

player *player_master::fetch_player_data(int id) {
    for (int i = 0; i < players.size(); i++) {
        if (players[i]->get_id() == id) return players[i];
    }

    return nullptr;
}


player_master::~player_master() {
    for (int i = 0; i < players.size(); i++) {
        delete[] players[i];
    }

    players.clear();
}


void player_master::draw_player(player *pl) {
    Texture2D* txt = texture_manager.grab_texture_pointer(default_texture_id);


    float scale = (float)txt->width/32;

    scale = 1;

    Vector2 drcd;

    drcd.x = round(pl->get_pos().x);
    drcd.y = round(pl->get_pos().y);

    DrawTextureEx(*txt, drcd, 0, scale, WHITE);
    DrawText(pl->get_name().c_str(), drcd.x, drcd.y - 12, 10, WHITE);
}

serialized_player player::serialize() {
    serialized_player ser;

    ser.pos = pos;
    ser.id = id;
    ser.stats = stats;

    std::strncpy(ser.name, name.c_str(), MAX_NAME_LENGTH - 1);
    ser.name[MAX_NAME_LENGTH - 1] = '\0';
    return ser;
}

void player::set_pos(Vector2 pos2) {
    pos = pos2;
}

bool player_master::does_player_exist(int id) {
    for (int i = 0; i < players.size(); i++) {
        if (players[i]->get_id() == id) return true;
    }

    return false;
}

void player::set_stats(stat_s st) {
    stats = st;
}


int player_master::add_player_from_serialised_player(serialized_player *spl) {
    player* new_player = new player;

    new_player->set_id(spl->id);

    new_player->set_name(spl->name);

    new_player->set_pos(spl->pos);

    new_player->set_stats(spl->stats);


    players.push_back(new_player);

    std::cout << "Player " << spl->name << " added" << std::endl;

    return players.size() - 1;
}

int player_master::remove_player(int id) {
    for (int i = 0; i < players.size(); i++) {
        if (players[i]->get_id() == id) {
            players.erase(players.begin() + i);

            return 0;
        }
    }

    return -1;
}
