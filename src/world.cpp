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