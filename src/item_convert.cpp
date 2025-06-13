#include "../include/item_convert.hpp"
#include "../include/networking.hpp"

bool item_convert_master::convert_item(item *itm, block table, player *pl) {
    if (network_manager.is_host()) {
        for (int i = 0; i < swap_ic_table.size(); i++) {
            if (swap_ic_table[i].org_item->item_id == itm->item_id && table.attr->item_id == swap_ic_table[i].table->item_id && pl->inv.does_have_item(itm, 1)) {
                pl->inv.take_item(itm, 1);

                //std::cout << "org_item: " << swap_ic_table[i].org_item->name << std::endl;

                //std::cout << "result_item: " << swap_ic_table[i].result_item->name << std::endl;

                pl->inv.give_item(swap_ic_table[i].result_item, 1);


                return true;
            }
        }
    }

    else {
        client_utls::convert_item(itm, item_manager.fetch_item_by_id(table.attr->item_id));
    }

    return false;
}

void item_convert_master::init() {
    std::ifstream file(ITEM_CONVERT_JSON_PATH);
    if (!file.is_open()) {
        std::cerr << "Failed to open json\n";
        std::exit(EXIT_FAILURE);
    }

    json j;
    file >> j;

    if (!j.is_array()) {
        std::cerr << "Expected JSON array at top level\n";
        std::exit(EXIT_FAILURE);
    }

    for (int i = 0; i < j.size(); i++) {
        swap_ic ic;

        ic.org_item = item_manager.fetch_item(j[i].value("item", "air"));
        //std::cout << j[i]["item"] << std::endl;
        ic.result_item = item_manager.fetch_item(j[i].value("result", "air"));
        //std::cout << j[i]["result"] << std::endl;
        //std::cout << ic.result_item->name << std::endl;
        ic.table = item_manager.fetch_item(j[i].value("table", "air"));
        //ic.table_state_min = item_manager.fetch_item(j[i].value("table_state_min", 0));
        //std::cout << j[i]["table"] << std::endl;

        swap_ic_table.push_back(ic);
    }
}
