#include "item_master.hpp"
#include "player.hpp"

#define ITEM_CONVERT_JSON_PATH "reasource/conversion_table.json"

struct swap_ic {
    item* table;

    item* org_item;

    item* result_item;

    int table_state_min;
};

class item_convert_master {
    private:
        std::vector<swap_ic> swap_ic_table;

public:
    bool convert_item(item* itm, block table, player* pl);

    void init();

};

extern item_convert_master item_convert_manager;