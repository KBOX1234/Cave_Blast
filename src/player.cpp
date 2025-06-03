#include "player.hpp"
#include "render.hpp"
#include "input.hpp"
#include "texture_master.hpp"
#include "imgui_window.hpp"
#include "networking.hpp"
#include "raymath.h"
#include "delta_time.hpp"
#include "inventory.hpp"

player::player() {
    pos = {0, 0};
    last_move = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();

}


void player::set_id(int id2) {
    id = id2;

    inv.set_player_id(id2);
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

void player::increase_angle(float amount, bool no_delta) {
    if (delta_time_master.can_game_continue() == true && no_delta == false) rotation = rotation + amount;
    else rotation = rotation + amount;
}

void player::zero_rotation(bool no_delta) {
    if (delta_time_master.can_game_continue() == true && no_delta == false) rotation = 0;
    else rotation = 0;
}

float player::get_rotation() {
    return rotation;
}



void player::move_player() {
    if (delta_time_master.can_game_continue() == true){
        Vector2 d;


        float rad = rotation * (M_PI / 180.0f);

        d.x = std::cos(rad) * speed;
        d.y = std::sin(rad) * speed;

        pos.x += d.x;
        pos.y += d.y;
    }
}
void player::move_player_back() {
    if (delta_time_master.can_game_continue() == true){


        Vector2 d;

        float rad = rotation * (M_PI / 180.0f);

        d.x = std::cos(rad) * speed;
        d.y = std::sin(rad) * speed;

        pos.x -= d.x;
        pos.y -= d.y;
    }
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
    myself = new player;
    inv_ui.set_inventory_pointer(&myself->inv);
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

    inv_ui.init();
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

    delete myself;
    for (int i = 0; i < players.size(); i++) {
        delete[] players[i];
    }

    players.clear();
}


void player_master::draw_player(player *pl) {
    Texture2D* txt = texture_manager.grab_texture_pointer(default_texture_id);

    // Calculate difference
    float dx = pl->pos.x - pl->interpolation.x;
    float dy = pl->pos.y - pl->interpolation.y;

    // Interpolation duration in seconds
    const float duration = 0.1f;

    // How much to move this frame (in units per second, scaled by frame time)
    float step_x = dx / duration * GetFrameTime();
    float step_y = dy / duration * GetFrameTime();

    // X
    if (std::abs(dx) > std::abs(step_x))
        pl->interpolation.x += step_x;
    else
        pl->interpolation.x = pl->pos.x;

    // Y
    if (std::abs(dy) > std::abs(step_y))
        pl->interpolation.y += step_y;
    else
        pl->interpolation.y = pl->pos.y;




    float scale = (float)txt->width/32;

    scale = 1;

    Vector2 drcd;

    drcd = pl->interpolation;

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

void player_master::update_predicted_player() {
    player* p = player_manager.get_host();

    p->set_pos(myself->get_pos());
    p->zero_rotation();
    p->increase_angle(myself->get_rotation());
}

int64_t player::last_move_tsmp() {
    return last_move;
}

void player::update_time_stamp() {
    last_move = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

bool player::is_valid_move(Vector2 pos2) {
    double ms_per_frame = 1000.0 / (double)GetFPS();

    // Assuming speed is pixels per frame
    double speed_pixels_per_ms = speed / ms_per_frame;

    speed_pixels_per_ms = speed_pixels_per_ms * 2;

    double current_time = std::chrono::duration<double, std::milli>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();

    double elapsed = current_time - last_move;  // last_move should also be in double ms

    double max_possible_distance = elapsed * speed_pixels_per_ms;

    if (Vector2Distance(pos2, pos) <= max_possible_distance) {
        return true;
    }

    return false;
}
Vector2 player::get_interpos() {
    return interpolation;
}

bool player::place_block(Vector2 pos){

    std::cout << "placing block\n";

    block new_block;
    new_block.state = 0;

    if(player_manager.inv_ui.get_current_item()->count < 1) return false;

    new_block.attr = player_manager.inv_ui.get_current_item()->item_i.block_type_ptr;
    player_manager.inv_ui.get_current_item()->count--;


    if(network_manager.is_host() == true){
        world.place_block(pos, new_block);
    }
    else{
        client_utls::place_block(player_manager.inv_ui.get_current_item()->item_i.name, pos, network_manager.get_server());
    }

    return true;
}

bool player::break_block(Vector2 pos){
    if(world.get_block(pos)->attr->item_id == item_manager.fetch_item("air")->item_id) return false;


    block blk;

    blk.attr = item_manager.fetch_item("air")->block_type_ptr;
    blk.state = 0;

    if(network_manager.is_host() == true){

        if(
            player_manager.inv_ui.current_item->item_i.strength < item_manager.fetch_item_by_id(world.get_block(pos)->attr->item_id)->strength
            && item_manager.fetch_item_by_id(world.get_block(pos)->attr->item_id)->strength > 0
    
        ) return false;

        if(player_manager.inv_ui.current_item->item_i.is_weapon == false && player_manager.inv_ui.current_item->item_i.item_id != 0) return false;
        

        item* itm = item_manager.fetch_item_by_id(world.get_block(pos)->attr->item_id);

        player_manager.myself->inv.give_item(itm, 1);

        world.place_block(pos, blk);
    }

    else{
        //std::cout << "sending break block request\n";

        std::string tool_name;

        if(player_manager.inv_ui.current_item->item_i.item_id == 0) tool_name = "air";

        else tool_name = player_manager.inv_ui.current_item->item_i.name;

        client_utls::break_block(network_manager.get_server(), pos, tool_name);
        //std::cout << player_manager.inv_ui.current_item->item_i.name << std::endl;
    }

    return true;
}