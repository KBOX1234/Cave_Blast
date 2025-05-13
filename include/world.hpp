#include "raylib.h"
#include "sphysics.h"
#include <cmath>

#define CHUNK_SIZE 16

struct block_attr{
    colideBox collision_box;
    short type;
};

struct block{
    char state;
    block_attr* attr;
};

class chunk{
    private:
        block* blocks;

        Vector2 global_pos;

        int chunk_id;

        chunk();
        ~chunk();

    public:

    void set_global_pos(Vector2 pos);


};

class world_class{
    private:

        block* world_data;

    public:

};