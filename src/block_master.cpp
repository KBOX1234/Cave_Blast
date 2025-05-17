#include "block_master.hpp"

int block_master::add_block_type(block_type b) {
    block_types.push_back(b);

    return block_types.size() - 1;
}

block_type* block_master::fetch_block_type(short type) {

    for(int i = 0; i < block_types.size(); i++){
        if(block_types[i].type == type) return &block_types[i];
    }

    return nullptr;

}