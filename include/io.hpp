#pragma once
#include <iostream>
#include <filesystem>
#include "player.hpp"
namespace fs = std::filesystem;


#define NOFILE "iul84y0ufp"
#define STRERROR "hdlslonfe"

class easy_file_ops{
    public:
        static std::string load_text_file(std::string filename);

        static int save_to_text_file(std::string data, std::string filename);
};

class save_master{
    private:
        std::string save_name;
        
        //save dir is just the folder with all the save data
        std::string save_dir;

        std::string get_home_dir();

        std::string get_save_dir(std::string save_name);

    public:
        save_master();

        int save_chunk(std::string json_data);

        int save_player(player* p);

        int save_npc(npc* npc_obj);

        int save_instance();
};

extern save_master save_manager;
