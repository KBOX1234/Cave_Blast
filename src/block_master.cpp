#include "block_master.hpp"

int block_master::add_block_type(const block_type& b) {
    block_types.push_back(std::make_unique<block_type>(b));
    return static_cast<int>(block_types.size() - 1);
}


block_type* block_master::fetch_block_type(int type) {
    for (const auto& bt : block_types) {
        if (bt->type == type)
            return bt.get();
    }
    return nullptr;
}
