#include "world.hpp"
#include "defines.hpp"
#include "rng.hpp"
#include "item_master.hpp"
#include "block_master.hpp"
#include "networking.hpp"
#include "player.hpp"

int world_class::look_up_chunk_index(Vector2 coord){

    //std::cout << "\nLooking up chunk: " << std::to_string(coord.x) << "," << std::to_string(coord.y) << std::endl;

    int x = (int)round(coord.x);
    int y = (int)round(coord.y);

    if (x < -2147483648 || y < -2147483648) {
        std::cout << "intager underflow\n";

        return -1;
    }

    int abs_x = std::abs(x);
    int abs_y = std::abs(y);



    //MAKE SURE TO CHECK THIS OTHERWISE YOU MIGHT CRASH

    //std::cout << "befor size check\n";
    if (abs_x >= MAX_TABLE_SIZE || abs_y >= MAX_TABLE_SIZE) {

        return -1;
    }
    //std::cout << "after size check\n";
    //std::cout << "abs size: " << std::to_string(abs_x) << ", " << std::to_string(abs_y) << std::endl;

    if (x >= 0 && y >= 0) return pos_x_pos_y[x][y];
    if (x <  0 && y >= 0) return neg_x_pos_y[abs_x][y];
    if (x >= 0 && y <  0) return pos_x_neg_y[x][abs_y];
    if (x <  0 && y <  0) return neg_x_neg_y[abs_x][abs_y];

    //Btw 1852142180 is the same as 'nerd' and fits into an integer.
    #define NERD 1852142180

    //satify compiler 
    return NERD;
}

bool world_class::is_chunk_index_valid(int index){
    if(index >= chunks.size()){
        #ifdef DEBUG
        std::cout << "index: " + std::to_string(index) + " is not valid becaus it is out of range\n";

        #endif

        return false;
    }

    return true;
}

chunk* world_class::get_chunk(Vector2 chunk_cord){
    int chunk_index = look_up_chunk_index(chunk_cord);

    if(is_chunk_index_valid(chunk_index) == true && chunk_index != -1){
        return &chunks[chunk_index];
    }

    return nullptr;
}

int world_class::add_look_up(Vector2 coord, int index){
    int x = (int)round(coord.x);
    int y = (int)round(coord.y);

    int abs_x = abs((int)round(coord.x));
    int abs_y = abs((int)round(coord.y));

    //MAKE SURE TO CHECK THIS OTHERWISE YOU MIGHT CRASH
    if (abs_x >= MAX_TABLE_SIZE|| abs_y >= MAX_TABLE_SIZE) return -1;

    if (x >= 0 && y >= 0) pos_x_pos_y[x][y] = index;
    if (x <  0 && y >= 0) neg_x_pos_y[abs_x][y] = index;
    if (x >= 0 && y <  0) pos_x_neg_y[x][abs_y] = index;
    if (x <  0 && y <  0) neg_x_neg_y[abs_x][abs_y] = index;

    return 0;
}

int world_class::add_chunk(chunk* chnk){

    chunks.push_back(*chnk);

    int latest_index = chunks.size() - 1;

    return add_look_up(chnk->get_chunk_pos(), latest_index);

}

Vector2 world_class::get_chunk_coord(Vector2 real_coord){
    Vector2 rtv;


    rtv.x = std::floor(real_coord.x / (float)CHUNK_SIZE);
    rtv.y = std::floor(real_coord.y / (float)CHUNK_SIZE);

    return rtv;
}

Vector2 world_class::get_sub_chunk_pos(Vector2 real_coord) {
    Vector2 rtv;

    float mod_x = std::fmod(real_coord.x, static_cast<float>(CHUNK_SIZE));
    if (mod_x < 0) mod_x += CHUNK_SIZE;
    rtv.x = mod_x;

    float mod_y = std::fmod(real_coord.y, static_cast<float>(CHUNK_SIZE));
    if (mod_y < 0) mod_y += CHUNK_SIZE;
    rtv.y = mod_y;

    return rtv;
}

//does not set state yet
int world_class::place_block(Vector2 pos, block b){

    if (item_manager.fetch_item_by_id(b.attr->item_id)->is_block == false) return -1;

    int chunk_index = look_up_chunk_index(get_chunk_coord(pos));

    if(chunk_index == -1) return -1;

    block_change bc;

    bc.blk_name = item_manager.get_item_name_by_id(b.attr->item_id);

    bc.pos = pos;

        network_manager.server_obj.add_block_change(bc);

        block_change blkch;

        blkch.blk_name = item_manager.get_item_name_by_id(b.attr->item_id);

        blkch.pos = pos;

        network_manager.server_obj.add_block_change(blkch);


    return chunks[chunk_index].set_block(b.attr, get_sub_chunk_pos(pos));
}

block* world_class::get_block(Vector2 pos){
    int chunk_index = look_up_chunk_index(get_chunk_coord(pos));

    if(chunk_index == -1) return nullptr;

    return chunks[chunk_index].get_block(get_sub_chunk_pos(pos));
}

int world_class::new_chunk_from_json(json j) {


    chunk chnk;
    //std::cout << j.dump(4);

    int return_v = chnk.new_chunk_from_json(j);

    //std::cout << "added new chunk from network\n";

    if (return_v == 0) {
        chunks.push_back(std::move(chnk));

        int chunk_id = j["chunk_id"];

        Vector2 chunk_pos;

        chunk_pos.x = j["pos"]["x"];
        chunk_pos.y = j["pos"]["y"];

        int latest_index = chunks.size() - 1;

        add_look_up(chunk_pos, latest_index);

    }

    return 0;

}

world_class::world_class() {
    for (int i = 0; i < MAX_TABLE_SIZE; i++) {
        for (int j = 0; j < MAX_TABLE_SIZE; j++) {
            pos_x_pos_y[i][j] = -1;
            neg_x_pos_y[i][j] = -1;
            pos_x_neg_y[i][j] = -1;
            neg_x_neg_y[i][j] = -1;
        }
    }
}

chunk *world_class::generate_chunk(Vector2 pos) {
    chunk* chnk = generator.generate_chunk(pos);

    if(pos.x == 0 && pos.y == 0){
        for(int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++){
            chnk->set_block_index(item_manager.fetch_item("air")->block_type_ptr, i);
        }
    }

    add_chunk(chnk);

    //delete chnk;

    return get_chunk(pos);
}


const block* world_class::chunk_buffer(Vector2 pos){

    chunk* tmp_chunk = get_chunk(pos);

    if(tmp_chunk == nullptr) {
        if (network_manager.is_host() == true) {
            tmp_chunk = generate_chunk(pos);
        }

        else {
            std::thread t(&client::fetch_chunk, &network_manager.client_obj, pos);
            t.detach();
            return nullptr;

        }
    }

    return tmp_chunk->blocks_buffer();
}

json world_class::serialize_chunk(Vector2 pos) {
    chunk* chnk = get_chunk(pos);

    return chnk->serialize_chunk();
}

int world_class::load_chunk(json j) {
    if (j.contains("pos")) {
        json pos = j["pos"];

        Vector2 chnk_pos;
        chnk_pos.x = pos["x"];
        chnk_pos.y = pos["y"];

        chunk* chnk = get_chunk(chnk_pos);

        chnk->new_chunk_from_json(j);

        return 0;
    }

    return -1;
}

void world_class::set_block_radius(block blk, int height, Vector2 pos) {
    int vertical_radius = height / 2;
    int horizontal_radius = vertical_radius; // You can customize this if you want ellipses

    for (int y = -vertical_radius; y <= vertical_radius; y++) {
        for (int x = -horizontal_radius; x <= horizontal_radius; x++) {
            float dist = sqrtf((x * x) + (y * y));
            if (dist <= vertical_radius) { // Fills the circle instead of drawing an edge
                world.place_block({pos.x + x, pos.y + y}, blk);
            }
        }
    }
}

void world_class::break_block(Vector2 pos){
    block blk;

    blk.attr = item_manager.fetch_item("air")->block_type_ptr;
    blk.state = 0;

    place_block(pos, blk);
}