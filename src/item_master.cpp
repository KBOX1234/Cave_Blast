#include "item_master.hpp"

#include <string>

#include "texture_master.hpp"
#include "rng.hpp"



int item_master::add_item(std::unique_ptr<item> i) {

    i->item_id = static_cast<short>(items.size());

    //quick fix 

    if(i->block_type_ptr == nullptr) i->block_type_ptr = new block_type;
    i->block_type_ptr->item_id = i->item_id;

    item_map[i->name] = i->item_id;

    items.push_back(std::move(i));

    return static_cast<int>(items.size() - 1);
}


item* item_master::fetch_item(const std::string& name) {
    auto it = item_map.find(name);
    if (it != item_map.end()) {
        return items[it->second].get();  // Dereference unique_ptr
    }
    return nullptr;
}


int item_master::load_items_from_json(json j) {
    int count = 0;

    for (auto& json_item : j) {
        auto new_item = std::make_unique<item>();

        new_item->is_liquid = json_item.value("is_liquid", false);
        new_item->is_solid = json_item.value("is_solid", false);
        new_item->is_usable = json_item.value("is_usable", false);
        new_item->is_consumable = json_item.value("is_consumable", false);
        new_item->is_wearable = json_item.value("is_wearable", false);
        new_item->is_block = json_item.value("is_block", false);
        new_item->is_weapon = json_item.value("is_weapon", false);
        new_item->is_ore = json_item.value("is_ore", false);
        new_item->chance_to_spawn = json_item.value("chance_to_spawn", 1);
        new_item->ore_radius = json_item.value("ore_radius", 1);
        new_item->strength = json_item.value("strength", 0);

        new_item->texture_path = json_item.value("texture_path", "");
        if (!new_item->texture_path.empty()) {
            new_item->texture_id = texture_manager.add_texture(new_item->texture_path, true);
        } else {
            new_item->texture_id = texture_manager.default_texture();
        }

        new_item->description = json_item.value("description", "no description");

        new_item->item_id = random_num.get_random_int();

        if (json_item.contains("name")) {
            new_item->name = json_item["name"];

            if (new_item->is_block) {
                block_type new_block;
                new_block.type = new_item->item_id;
                new_block.item_id = new_item->item_id;

                if (json_item.contains("block_texture_path")) {

                    new_block.texture_id = texture_manager.add_texture(json_item["block_texture_path"]);

                } else {
                    new_block.texture_id = texture_manager.default_texture();
                }

                block_manager.add_block_type(new_block);
                new_item->block_type_ptr = block_manager.fetch_block_type(new_block.type);
            }



            add_item(std::move(new_item));

            count++;
        } else {

        }
    }

    std::cout << "(ITEM_MANAGER): Loaded " << std::to_string(count) << " items\n";

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

item* item_master::fetch_item_by_id(int id) {
    if (id >= 0 && id < items.size()) {
        return items[id].get();
    }
    return nullptr;
}

std::string item_master::get_item_name_by_id(int id) {
    for (int i = 0; i < items.size(); i++) {
        if (items[i]->item_id == id) {
            return items[i]->name;
        }
    }

    return "null";
}

std::vector<std::string> item_master::get_existing_items() {
    std::vector<std::string> item_names;
    for (const auto& it : items) {
        if (it) {
            item_names.push_back(it->name);
        }
    }

    return item_names;
}

std::vector<item> item_master::get_all_ores(){
    std::vector<item> rtv;

    for(int i = 0; i < items.size(); i++){
        if(items[i].get()->is_ore == true){
            rtv.push_back(*items[i].get());

            //std::cout << "(get_all_ores): chance to spawn is: " << std::to_string(items[i].get()->chance_to_spawn) << std::endl;
        }
    }

    return rtv;
}