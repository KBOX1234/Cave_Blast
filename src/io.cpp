#include "../include/io.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include "json.hpp"
#include "npc.hpp"
#include "world.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;


std::string easy_file_ops::load_text_file(std::string filename){
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Unable to open file.\n";
        return NOFILE;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();

}

int easy_file_ops::save_to_text_file(std::string data, std::string filename){
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Unable to open file for writing: " + filename + "\n";
        return -1;
    }

    file << data;

    file.close();

    return 0;
}

std::string save_master::get_home_dir(){
    #ifdef _WIN32
        const char* home = std::getenv("HOMEPATH");
    #else 
        const char* home = std::getenv("HOME");
    #endif

    if(home == nullptr) return STRERROR;

    return home;
}

#define GAME_FOLDER_NAME "cave_blast"

std::string save_master::get_save_dir(std::string save_name){
    std::string home = get_home_dir();

    if(home == STRERROR){
        return STRERROR;
    }

    std::string save_dir = home + "/" + GAME_FOLDER_NAME + "/" + save_name;

    return save_dir;
}

save_master::save_master(){
    save_name = "dev";

    save_dir = get_save_dir(save_name);
}

#define CHUNK_DIR "chunks"

int save_master::save_chunk(std::string json_data){

    json chunk_json = json::parse(json_data);

    Vector2 c_pos;

    c_pos.x = chunk_json["pos"]["x"];
    c_pos.y = chunk_json["pos"]["y"];

    std::string fname = std::to_string((int)round(c_pos.x)) + "," + std::to_string((int)round(c_pos.y)) + ".cjd";

    std::string full_save_path = save_dir + "/" + CHUNK_DIR + "/" + fname;

    return easy_file_ops::save_to_text_file(json_data, full_save_path);
}

#define PLAYERS_DIR "players"

int save_master::save_player(player* p){
    json j;

    json player_stat;
    serialized_player sp;

    json inventory;

    sp = p->serialize();

    player_stat["pos"]["x"] = sp.pos.x;
    player_stat["pos"]["y"] = sp.pos.y;

    player_stat["stats"]["health"] = sp.stats.health;
    player_stat["stats"]["hunger"] = sp.stats.hunger;
    player_stat["stats"]["thirst"] = sp.stats.thirst;

    player_stat["id"] = sp.id;

    player_stat["angle"] = sp.angle;

    player_stat["name"] = sp.name;

    inventory = json::parse(p->inv.export_inventory_json());

    j["player_dat"] = player_stat;
    j["inventory"] = inventory;
    

    std::string fname = std::to_string(sp.id) + ".pdat";

    std::string full_p_save_f = save_dir + "/" + PLAYERS_DIR + "/" + fname;

    return easy_file_ops::save_to_text_file(j.dump(), full_p_save_f);

}

#define NPC_DIR "npcs"

int save_master::save_npc(npc *npc_obj) {
    std::string json_str = npc_manager.serialize_npc_to_json(npc_obj);

    json j = json::parse(json_str);

    int id = j["id"];

    std::string fname = std::to_string(id) + ".npcdat";

    std::string full_p_save_f = save_dir + "/" + NPC_DIR + "/" + fname;

    return easy_file_ops::save_to_text_file(j.dump(), full_p_save_f);
}


int save_master::save_instance(){
    for(int i = 0; i < world.chunks.size(); i++){
        save_chunk(world.chunks[i].serialize_chunk().dump());
    }

    for(int i = 0; i < player_manager.players.size(); i++){
        save_player(player_manager.players[i]);
    }

    for (int i = 0; i < npc_manager.npcs.size(); i++) {
        save_npc(npc_manager.npcs[i]);
    }

    return 0;
}

void save_master::load_chunks() {
    world.reset_chunks();

    std::string dir = save_dir + "/" + CHUNK_DIR + "/";

    try {
        for (const auto& entry : fs::directory_iterator(dir)) {
            try {
                auto data = json::parse(easy_file_ops::load_text_file(entry.path()));
                world.new_chunk_from_json(data);
            } catch (const json::parse_error& e) {
                std::cerr << "(SAVE_MANAGER): Failed to parse " << entry.path() << ": " << e.what() << '\n';
            }

        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "(SAVE_MANAGER): Error: " << e.what() << '\n';
    }
}

void save_master::load_npcs() {
    npc_manager.clear_npcs();


    std::string dir = save_dir + "/" + NPC_DIR + "/";

    try {
        for (const auto& entry : fs::directory_iterator(dir)) {
            try {
                auto data = json::parse(easy_file_ops::load_text_file(entry.path()));
                npc_manager.load_serialized_npc_from_json(data.dump());
            } catch (const json::parse_error& e) {
                std::cerr << "(SAVE_MANAGER): Failed to parse " << entry.path() << ": " << e.what() << '\n';
            }

        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "(SAVE_MANAGER): Error: " << e.what() << '\n';
    }
}

void save_master::load_players() {

}

void save_master::load_save() {
    load_chunks();
    load_npcs();
    load_players();
}



