#pragma once
#include <iostream>
#include <filesystem>

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

        std::string save_dir;

        std::string get_home_dir();

        std::string get_save_dir();

    public:
        save_master();

        int save_chunk(std::string json_data);

        int save_player(std::string json_data);
};
