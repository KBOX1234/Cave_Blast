#include "item_master.hpp"

#include <string>

#include "texture_master.hpp"
#include "../include/block_master.hpp"

texture_master texture_manager;

int item_master::add_item(item i) {

    i.item_id = items.size();

    items.push_back(i);

    int index = items.size() - 1;

    item_map[i.name] = index;

    return index;
}

item* item_master::fetch_item(std::string name) {
    auto it = item_map.find(name);

    if(it != item_map.end()){
        int index = it->second;

        return &items[index];
    }

    return nullptr;
}

int item_master::load_items_from_json(json j) {
    int count = 0;
    for (auto& json_item : j) {
        item new_item;
        new_item.item_id = items.size();



        new_item.is_liquid = json_item.value("is_liquid", false);
        new_item.is_solid = json_item.value("is_solid", false);
        new_item.is_usable = json_item.value("is_usable", false);
        new_item.is_consumable = json_item.value("is_consumable", false);
        new_item.is_wearable = json_item.value("is_wearable", false);
        new_item.is_block = json_item.value("is_block", false);
        new_item.is_weapon = json_item.value("is_weapon", false);

        new_item.texture_path = json_item.value("texture_path", "");
        if (!new_item.texture_path.empty()) {
            new_item.texture_id = texture_manager.add_texture(new_item.texture_path, true);
        } else {
            new_item.texture_id = texture_manager.default_texture();
        }

        new_item.description = json_item.value("description", "no description");

        if (json_item.contains("name")) {
            new_item.name = json_item["name"];
            add_item(new_item);
            count++;
        } else {
            std::cout << "Item name not found\n";
        }

        if (json_item.contains("is_block")) {
            block_type new_block;
            new_block.type = new_item.item_id;
            new_block.item_id = new_item.item_id;

            if (json_item.contains("block_texture_path")) {
                new_block.texture_id = texture_manager.add_texture(json_item["block_texture_path"]);
            } else new_block.texture_id = texture_manager.default_texture();

            int index = block_manager.add_block_type(new_block);


            new_item.block_type_ptr = block_manager.fetch_block_type(new_block.type);

        }
    }
    return count;
}

int item_master::load_item_declaration_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open items.json\n";
        return -1;
    }

    json j;
    file >> j;

    if (!j.is_array()) {
        std::cerr << "Expected JSON array at top level\n";
        return -1;
    }

    return load_items_from_json(j);
}

item* item_master::fetch_item_by_id(short id) {
    return &items[id];
}