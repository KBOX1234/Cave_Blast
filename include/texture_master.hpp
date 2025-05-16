#include <vector>
#include <iostream>
#include <chrono>
#include "raylib.h"

struct texture_archive{
    int id;
    Texture2D texture;
    std::string origin;
    bool loaded;

    std::time_t expiration;
};

class texture_master{
    private:
        std::vector<texture_archive> textures;

    public:

        int add_texture(std::string path);

        Texture2D* grab_texture_pointer(int id);

        void update();
};