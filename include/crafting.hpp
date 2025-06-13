#pragma once

#define NO_TABLE 0
#include <vector>

#include "inventory.hpp"
#include "item_master.hpp"
#include "player.hpp"

#define CRAFT_RECIPE_JSON_PATH "reasource/crafter_recipes.json"

struct craft_mix {
    char table_type;
    std::vector<inventory_slot> item_reqs;
    inventory_slot result;
    int index;
};

class server_utls;

class crafting_master {
        friend class server_utls;
    private:

        std::vector<craft_mix> crafting_recipes;

        bool does_player_have_requirements(player* pl, craft_mix* cm);

        void deduct_items(player* pl, craft_mix* cm);

    public:

        void init();

        bool craft_item(int craft_index, player* pl);

        std::vector<craft_mix> get_avalible_crafting_recipies(player* pl);

};

extern crafting_master crafting_manager;