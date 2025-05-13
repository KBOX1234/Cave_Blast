#include "raylib.h"
#include "sphysics.h"
#include <cmath>
#include <string>

#define CHUNK_SIZE 16

struct block_attr{
    colideBox collision_box;
    short type;
    int texture_id;
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

    int set_block(block b, Vector2 pos);

    int set_block_index(block b, int index);

    block* get_block(Vector2 pos);
    
    block* get_block_index(int index);


};

class world_class{
    private:

        block* world_data;

    public:

};