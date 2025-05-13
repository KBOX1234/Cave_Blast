#include <vector>
#include "raylib.h"

struct texture_archive{
    int id;
    Texture2D texture;
};

class texture_master{
    private:
        std::vector<texture_archive> textures;

    public:

        void add_texture(Texture2D img, int id);

        Texture2D* grab_texture_pointer(int id);
};