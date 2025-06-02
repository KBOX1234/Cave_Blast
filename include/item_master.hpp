#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "json.hpp"
#include "block_master.hpp"

using json = nlohmann::json;


struct item {
    std::string name;
    std::string description;
    std::string texture_path;

    int item_id;
    int texture_id;

    bool is_weapon;
    bool is_block;
    bool is_wearable;
    bool is_consumable;
    bool is_usable;
    bool is_solid;
    bool is_liquid;
    bool is_ore;

    //if ore is true
    float chance_to_spawn;
    int ore_radius;

    block_type* block_type_ptr;
};

class item_master {
private:
    std::unordered_map<std::string, int> item_map;

    std::vector<std::unique_ptr<item>> items;

    int add_item(std::unique_ptr<item> i);

public:

    item* fetch_item(const std::string& name);

    item* fetch_item_by_id(int id);

    int load_items_from_json(json j);

    int load_item_declaration_file(const std::string& path);

    std::vector<std::string> get_existing_items();

    std::string get_item_name_by_id(int id);

    std::vector<item> get_all_ores();

};



extern item_master item_manager;