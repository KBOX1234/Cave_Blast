#include "player.hpp"
#include "render.hpp"
#include "input.hpp"
#include "texture_master.hpp"
#include "imgui_window.hpp"
#include "networking.hpp"
#include "raymath.h"
#include "delta_time.hpp"
#include "inventory.hpp"
#include "colide.hpp"
#include "lighting.hpp"
#include "io.hpp"

player::player() {

    pos = new Vector2;


    //place in the middle of spawn chunk
    *pos = {2 * BLOCK_SIZE, 2 * BLOCK_SIZE};
    last_move = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();

    Color c;
    rotation = 0;
    c.a = 255;
    c.b = 255;
    c.g = 255;
    c.r = 255;



    light_index = light_manager.add_light(c, 150, {0, 0}, 0.5, 0);

}

void player::update_colide_box(){
    box.a = colide::v2p(*pos);

    box.b.x = pos->x + PLAYER_WIDTH;

    box.b.y = pos->y + PLAYER_HEIGHT;
}


void player::set_id(int id2) {
    id = id2;

    inv.set_owner_id(id2);
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
    if (!delta_time_master.can_game_continue()) return;

    Vector2 next = get_move_target();

    if (is_valid_move_2(next)) {
        last_pos = *pos;
        *pos = next;
        update_colide_box();
        update_time_stamp();
        amsl.set_pos(pos);
    } else {

    }

    light_manager.update_light_pos(light_index, *pos);
}

bool player::is_valid_move_2(Vector2 pos2) {

    const float box_width = PLAYER_WIDTH;
    const float box_height = PLAYER_HEIGHT;

    colideBox test_box;
        test_box.a = colide::v2p(pos2);
        //test_box.a.x = test_box.a.x - box_width;


        Vector2 bottom_right = { pos2.x + box_width, pos2.y + box_height};
        test_box.b = colide::v2p(bottom_right);


    int start_block_x = (int)(pos2.x / BLOCK_SIZE) - 1;
    int start_block_y = (int)(pos2.y / BLOCK_SIZE) - 1;
    int end_block_x = (int)((pos2.x + box_width) / BLOCK_SIZE) + 1;
    int end_block_y = (int)((pos2.y + box_height) / BLOCK_SIZE) + 1;


    for (int by = start_block_y; by <= end_block_y; ++by) {
        for (int bx = start_block_x; bx <= end_block_x; ++bx) {
            Vector2 block_vec = { (float)bx - 1, (float)by};
            block* blk = world.get_block(block_vec);

            if (blk != nullptr && blk->attr != nullptr) {
                auto item = item_manager.fetch_item_by_id(blk->attr->item_id);
                if (item->is_block == false || item->is_solid == false) continue;


                Vector2 block_top_left = { (float)(bx * BLOCK_SIZE), (float)(by * BLOCK_SIZE) };

                Vector2 block_bottom_right = {
                    block_top_left.x + BLOCK_SIZE,
                    block_top_left.y + BLOCK_SIZE
                };

                colideBox block_box;
                block_box.a = colide::v2p(block_top_left);
                block_box.b = colide::v2p(block_bottom_right);

                // 4. Check for collision
                if (doesBoxAndBoxColide(&test_box, &block_box)) {
                    return false;
                }
            }
        }
    }

    return true; // No collisions
}



Vector2 player::get_move_target() {
    Vector2 d;
    float rad = rotation * (M_PI / 180.0f);
    d.x = std::cos(rad) * speed;
    d.y = std::sin(rad) * speed;

    return { pos->x + d.x, pos->y + d.y };
}


void player::move_player_back() {
    if (delta_time_master.can_game_continue() == true){

        block* blk1 = world.get_block(get_block_pos());
        block* blk2 = world.get_block({get_block_pos().x, get_block_pos().y + 1});

        if(blk1 == nullptr || blk2 == nullptr) return;

        Vector2 d;

        float rad = rotation * (M_PI / 180.0f);

        d.x = std::cos(rad) * speed;
        d.y = std::sin(rad) * speed;

        Vector2 predicted_pos = {round(pos->x - d.x), round(pos->y - d.y)};

        Vector2 block_pos = {round(predicted_pos.x / BLOCK_SIZE)};

        pos->x -= d.x;
        pos->y -= d.y;
    }
}

Vector2 player::get_pos() {
    return *pos;
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

    serialized_player sp;

    sp.id = players.size();

    memcpy(sp.name, name.c_str(), MAX_NAME_LENGTH);

    sp.angle = 0;
    sp.pos = {0, 0};
    sp.stats = {0, 0, 0};

    std::cout << "(PLAYER_MANAGER): Player: " << name << " has joined" << std::endl;

    return add_player_from_serialised_player(&sp);
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

    animation_manager.link_ams_linker(&myself->amsl);

    myself->amsl.load_animation_from_json(easy_file_ops::load_text_file("reasource/animations/player/animate.json"));
    animation_manager.link_ams_linker(&myself->amsl);
    myself->amsl.play_animation("walk", true);
    myself->amsl.set_pos(&myself->interpolation);

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
        //fix me
        //delete[] players[i];
    }

    players.clear();
}


void player_master::draw_player(player *pl) {
    //Texture2D* txt = texture_manager.grab_texture_pointer(default_texture_id);

    // Calculate difference
    float dx = pl->pos->x - pl->interpolation.x;
    float dy = pl->pos->y - pl->interpolation.y;

    // Interpolation duration in seconds
    const float duration = 0.1f;

    // How much to move this frame (in units per second, scaled by frame time)
    float step_x = dx / duration * GetFrameTime();
    float step_y = dy / duration * GetFrameTime();

    // X
    if (std::abs(dx) > std::abs(step_x))
        pl->interpolation.x += step_x;
    else
        pl->interpolation.x = pl->pos->x;

    // Y
    if (std::abs(dy) > std::abs(step_y))
        pl->interpolation.y += step_y;
    else
        pl->interpolation.y = pl->pos->y;




    float scale = 4;

    scale = 1;

    Vector2 drcd;

    drcd = pl->interpolation;

    //DrawTextureEx(*txt, drcd, 0, scale, WHITE);
    DrawText(pl->get_name().c_str(), drcd.x, drcd.y - 12, 10, WHITE);
}

serialized_player player::serialize() {
    serialized_player ser;

    ser.pos = *pos;
    ser.id = id;
    ser.stats = stats;

    std::strncpy(ser.name, name.c_str(), MAX_NAME_LENGTH - 1);
    ser.name[MAX_NAME_LENGTH - 1] = '\0';
    return ser;
}

void player::set_pos(Vector2 pos2) {

    *pos = pos2;
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

    new_player->amsl.load_animation_from_json(easy_file_ops::load_text_file("reasource/animations/player/animate.json"));
    animation_manager.link_ams_linker(&new_player->amsl);
    new_player->amsl.play_animation("walk", true);
    new_player->amsl.set_pos(&new_player->interpolation);

    players.push_back(new_player);

    std::cout << "(PLAYER_MANAGER): Player: " << spl->name << " has joined" << std::endl;

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

    if (Vector2Distance(pos2, *pos) <= max_possible_distance) {
        return true;
    }

    return false;
}
Vector2 player::get_interpos() {
    return interpolation;
}

bool player::place_block(Vector2 pos){

    if (item_manager.fetch_item_by_id(world.get_block(pos)->attr->item_id)->name != "air") return false;

    //std::cout << "placing block\n";

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

Vector2 player::get_block_pos(){
    Vector2 rtv;

    rtv.x = round(pos->x / BLOCK_SIZE);
    rtv.y = round(pos->y / BLOCK_SIZE);

    return rtv;
}

int player::get_light_index(){
    return light_index;
}

void player_master::give_player_item(item *i, int count, int player_id) {
    if (player_id == myself->id) {
        if (network_manager.is_host() == true) {
            myself->inv.give_item(i, count);

            //else it ant my responsibility
        }
    }

    else {
        if (network_manager.is_host() == true) {
            fetch_player_data(player_id)->inv.give_item(i, count);
        }

        //else it ant my responsibility
    }
}
