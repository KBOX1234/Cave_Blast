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

bool world_class::approx_equal(float a, float b, float epsilon) {
    return std::fabs(a - b) < epsilon;
}

// Equality operator for chunk_table_entry
bool world_class::chunk_table_entry::operator==(const chunk_table_entry& other) const {
    return chunk_ptr == other.chunk_ptr &&
        world_class::approx_equal(regon.a.x, other.regon.a.x) &&
        world_class::approx_equal(regon.a.y, other.regon.a.y) &&
        world_class::approx_equal(regon.b.x, other.regon.b.x) &&
        world_class::approx_equal(regon.b.y, other.regon.b.y);
}

// Hash function for chunk_table_entry
std::size_t world_class::chunk_table_entry_hash::operator()(const chunk_table_entry& entry) const {
    std::size_t h1 = std::hash<float>()(entry.regon.a.x);
    std::size_t h2 = std::hash<float>()(entry.regon.a.y);
    std::size_t h3 = std::hash<float>()(entry.regon.b.x);
    std::size_t h4 = std::hash<float>()(entry.regon.b.y);
    std::size_t h5 = std::hash<chunk*>()(entry.chunk_ptr); // Hash the pointer

    return (((((h1 ^ (h2 << 1)) ^ (h3 << 1)) ^ (h4 << 1)) ^ (h5 << 1)));
}