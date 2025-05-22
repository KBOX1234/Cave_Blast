#include "world.hpp"


chunk::chunk(){
    chunk_id = random_num.get_random_int();
    blocks.resize(CHUNK_SIZE*CHUNK_SIZE);
}

chunk::~chunk(){

}

void chunk::set_global_pos(Vector2 pos){

    global_pos.x = round(pos.x);
    global_pos.y = round(pos.y);

    #ifdef DEBUG
    std::cout << "Global Position Set: " + to_string((int)global_pos.x) + ", " + to_string((int)global_pos.y) + "\n";
    #endif
}

int chunk::set_block(block_type* b, Vector2 pos){
    int index = ((int)round(pos.y)*CHUNK_SIZE) + (int)round(pos.x);

        //std::cout << "\nindex = " + std::to_string(index) + "\n";


    if(index > CHUNK_SIZE*CHUNK_SIZE || index < 0){
        return -1;

        #ifdef DEBUG
            std::cout << "Can't place block there!\nOut of Bounds: " + to_string((int)pos.x) + ", " + to_string((int)pos.y) + "\n";
        #endif
    }

    block new_blk;
    new_blk.state = 0;
    new_blk.attr = b;

    blocks[index] = new_blk;

    return 0;
}

int chunk::set_block_index(block_type* b, int index){
    if(index > CHUNK_SIZE*CHUNK_SIZE){
        return -1;

        #ifdef DEBUG
            std::cout << "Can't place block there!\nOut of Bounds: " + to_string((int)pos.x) + ", " + to_string((int)pos.y) + "\n";
        #endif
    }

    if (index >= blocks.size()) {
        blocks.resize(index + 1);
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
        std::string name = item_manager.get_item_name_by_id(blocks[i].attr->item_id);
        if (name == "null") {
            block_data[i] = blocks[i].attr->item_id;

        }
        else {
            block_data[i] = name;
        }
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

        json data = j["block_data"];
        for(int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++) {
            blocks[i].attr->type = data[i];
        }
    }

    else return NO_BLOCKS;

    return 0;
}

const block* chunk::blocks_buffer() const {
    return blocks.data();
}

