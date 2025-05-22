#pragma once
#include <vector>
#include <sphysics.h>
#include <memory>


struct block_type{
    colideBox collision_box;
    short type;
    int texture_id;
    int item_id;
};

struct block{
    char state;
    block_type* attr;
};

class block_master {
    private:
    std::vector<std::unique_ptr<block_type>> block_types;

    public:

    int add_block_type(const block_type& b);

    block_type* fetch_block_type(int type);



};

extern block_master block_manager;
