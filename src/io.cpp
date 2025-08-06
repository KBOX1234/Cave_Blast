#include "../include/io.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include "json.hpp"
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
        std::cerr << "Unable to open file for writing.\n";
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

std::string save_master::get_save_dir(std::string save_name){
    std::string home = get_home_dir();

    if(home == STRERROR){
        return STRERROR;
    }

    std::string save_dir = home + "/" + save_name;

    return save_dir;
}

save_master::save_master(){
    save_name = "dev";

    save_dir = get_save_dir(save_name);
}

#define CHUNK_DIR "chunks"

int save_master::save_chunk(std::string json_data){
    std::string chnk_save_name;

    json chunk_json = json::parse(json_data);

    
}
