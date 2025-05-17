#include <vector>
#include <sphysics.h>
#pragma once

struct block_type{
    colideBox collision_box;
    short type;
    int texture_id;
    short item_id;
};

struct block{
    char state;
    block_type* attr;
};

class block_master {
    private:
    std::vector<block_type> block_types;

    public:

    int add_block_type(block_type b);

    block_type* fetch_block_type(short type);


};

extern block_master block_manager;