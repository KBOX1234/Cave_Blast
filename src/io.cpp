#include "../include/io.hpp"
#include <fstream>
#include <sstream>
#include <string>

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
