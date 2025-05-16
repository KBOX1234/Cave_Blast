#include "world.hpp"

#include <iostream>

#include "rng.hpp"

chunk::chunk(){
    blocks = new block[CHUNK_SIZE*CHUNK_SIZE];
    chunk_id = random_num.get_random_int();
}

chunk::~chunk(){
    delete[] blocks;
}

void chunk::set_global_pos(Vector2 pos){

    global_pos.x = round(pos.x);
    global_pos.y = round(pos.y);

    #ifdef DEBUG
    std::cout << "Global Position Set: " + to_string((int)global_pos.x) + ", " + to_string((int)global_pos.y) + "\n";
    #endif
}

int chunk::set_block(block b, Vector2 pos){
    int index = ((int)round(pos.y)*CHUNK_SIZE) + (int)round(pos.x);

    if(index > CHUNK_SIZE*CHUNK_SIZE){
        return -1;

        #ifdef DEBUG
            std::cout << "Can't place block there!\nOut of Bounds: " + to_string((int)pos.x) + ", " + to_string((int)pos.y) + "\n";
        #endif
    }

    blocks[index] = b;

    return 0;
}

int chunk::set_block_index(block_type* b, int index){
    if(index > CHUNK_SIZE*CHUNK_SIZE){
        return -1;

        #ifdef DEBUG
            std::cout << "Can't place block there!\nOut of Bounds: " + to_string((int)pos.x) + ", " + to_string((int)pos.y) + "\n";
        #endif
    }

    blocks[index].state = 0;
    blocks[index].attr = b;

    return 0;
}

block* chunk::get_block(Vector2 pos){
    int index = ((int)round(pos.y)*CHUNK_SIZE) + (int)round(pos.x);

    if(index > CHUNK_SIZE*CHUNK_SIZE){
        return nullptr;

        #ifdef DEBUG
            std::cout << "Can't place block there!\nOut of Bounds: " + to_string((int)pos.x) + ", " + to_string((int)pos.y) + "\n";
        #endif
    }

    return &blocks[index];

}

block* chunk::get_block_index(int index){

    if(index > CHUNK_SIZE*CHUNK_SIZE){
        return nullptr;

        #ifdef DEBUG
            std::cout << "Can't place block there!\nOut of Bounds: " + to_string((int)pos.x) + ", " + to_string((int)pos.y) + "\n";
        #endif
    }

    return &blocks[index];

}

Vector2 chunk::get_chunk_pos(){
    return global_pos;
}

json chunk::serialize_chunk() {
    json j;
    json pos;
    pos["x"] = global_pos.x;
    pos["y"] = global_pos.y;

    j["pos"] = pos;
    j["chunk_id"] = chunk_id;

    json block_data;
    //only shares block type val for simplicity and size
    for(int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++) {
        block_data[i] = blocks[i].attr->type;
    }
    j["block_data"] = block_data;

    return j;
}

#define NO_POS -1
#define NO_CHUNK_ID -2
#define NO_BLOCKS -3
int chunk::new_chunk_from_json(json j) {
    if (j.contains("pos")) {
        json pos = j["pos"];
        Vector2 tmp_pos;
        tmp_pos.x = pos["x"];
        tmp_pos.y = pos["y"];

        set_global_pos(tmp_pos);
    }

    else return NO_POS;

    if (j.contains("chunk_id")) {
        chunk_id = j["chunk_id"];
    }
    else return NO_CHUNK_ID;

    if (j.contains("block_data")) {

        if (blocks == nullptr) {
            std::cout << "Can't deserialize chunk blocks becaus the world data (blocks var) is not allocated.\nAllocating now\n";
            blocks = new block[CHUNK_SIZE*CHUNK_SIZE];
        }

        json data = j["block_data"];
        for(int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++) {
            blocks[i].attr->type = data[i];
        }
    }

    else return NO_BLOCKS;

    return 0;
}

int world_class::look_up_chunk_index(Vector2 coord){
    int x = (int)round(coord.x);
    int y = (int)round(coord.y);

    int abs_x = abs((int)round(coord.x));
    int abs_y = abs((int)round(coord.y));

    //MAKE SURE TO CHECK THIS OTHERWISE YOU MIGHT CRASH
    if (abs_x >= MAX_TABLE_SIZE|| abs_y >= MAX_TABLE_SIZE) return -1;

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

    if(is_chunk_index_valid(chunk_index) == true){
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

int world_class::place_block(Vector2 pos, block b){

    int chunk_index = look_up_chunk_index(get_chunk_coord(pos));

    if(chunk_index == -1) return -1;

    return chunks[chunk_index].set_block(b, get_sub_chunk_pos(pos));
}

block* world_class::get_block(Vector2 pos){
    int chunk_index = look_up_chunk_index(get_chunk_coord(pos));

    if(chunk_index == -1) return nullptr;

    return chunks[chunk_index].get_block(get_sub_chunk_pos(pos));
}