#include "world.hpp"
#include "rng.hpp"

chunk::chunk(){
    blocks = new block[CHUNK_SIZE*CHUNK_SIZE];
    chunk_id = random_num.get_random_int();
}

chunk::~chunk(){
    delete blocks;
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

int chunk::set_block_index(block b, int index){
    if(index > CHUNK_SIZE*CHUNK_SIZE){
        return -1;

        #ifdef DEBUG
            std::cout << "Can't place block there!\nOut of Bounds: " + to_string((int)pos.x) + ", " + to_string((int)pos.y) + "\n";
        #endif
    }

    blocks[index] = b;

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