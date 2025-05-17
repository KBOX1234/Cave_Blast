#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

#include "json.hpp"

#pragma once

using json = nlohmann::json;

#include "texture_master.hpp"

struct item {
    std::string name;
    std::string description;
    std::string texture_path;

    short item_id;
    int texture_id;

    bool is_weapon;
    bool is_block;
    bool is_wearable;
    bool is_consumable;
    bool is_usable;
    bool is_solid;
    bool is_liquid;
};

class item_master {
private:
    std::unordered_map<std::string, int> item_map;

    std::vector<item> items;

    int add_item(item i);

public:

    item* fetch_item(std::string name);

    int load_items_from_json(json j);

    int load_item_declaration_file(const std::string& path);

};



extern item_master item_manager;