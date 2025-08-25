#include "../include/crafting.hpp"
#include "../include/networking.hpp"
#include <string>

void crafting_master::init() {
    std::ifstream file(CRAFT_RECIPE_JSON_PATH);
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

    int count = 0;

    for (auto& json_item : j) {
        craft_mix cm;

        if (json_item.contains("items")) {
            for (auto& json_item2 : json_item["items"]) {
                inventory_slot invsl;

                invsl.item_i = *item_manager.fetch_item(json_item2.value("name", "air"));

                invsl.count = json_item2.value("count", 0);

                cm.item_reqs.push_back(invsl);
            }

            inventory_slot result_s;

            result_s.item_i = *item_manager.fetch_item(json_item.value("result", "air"));

            result_s.count = 1;

            cm.result = result_s;

            cm.index = crafting_recipes.size();

            crafting_recipes.push_back(cm);
        }

        count++;
    }

    std::cout << "(CRAFTING_MANAGER): Loaded " << std::to_string(count) << " recipies\n";
}

bool crafting_master::craft_item(int craft_index, player *pl) {
    if (does_player_have_requirements(pl, &crafting_recipes[craft_index]) == false) {
        return false;
    }

    deduct_items(pl, &crafting_recipes[craft_index]);

    player_manager.give_player_item(&crafting_recipes[craft_index].result.item_i, crafting_recipes[craft_index].result.count, pl->get_id());

    if (network_manager.is_host() == false && pl->get_id() == player_manager.myself->get_id()) {
        client_utls::craft_item(craft_index);
    }

    return true;
}

void crafting_master::deduct_items(player *pl, craft_mix *cm) {
    for (int i = 0; i < cm->item_reqs.size(); i++) {

        pl->inv.take_item(&cm->item_reqs[i].item_i, cm->item_reqs[i].count);

    }
}

bool crafting_master::does_player_have_requirements(player *pl, craft_mix *cm) {
    for (int i = 0; i < cm->item_reqs.size(); i++) {
        if (pl->inv.does_have_item(&cm->item_reqs[i].item_i, cm->item_reqs[i].count) == false) return false;
    }

    return true;
}

std::vector<craft_mix> crafting_master::get_avalible_crafting_recipies(player *pl) {
    std::vector<craft_mix> rtv;

    for (int i = 0; i < crafting_recipes.size(); i++) {
        if (does_player_have_requirements(pl, &crafting_recipes[i])) {
            rtv.push_back(crafting_recipes[i]);
        }

    }

    return rtv;
}


