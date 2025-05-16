#include "raylib.h"
#include "sphysics.h"
#include <cmath>
#include <string>
#include <vector>
#include <unordered_set>
#include <math.h>
#include "json.hpp"
#include "defines.hpp"
using json = nlohmann::json;
#define CHUNK_SIZE 16

struct block_type{
    colideBox collision_box;
    short type;
    int texture_id;
};

struct block{
    char state;
    block_type* attr;
};

class chunk{
    private:
        block* blocks;

        Vector2 global_pos;

        int chunk_id;

    public:

    chunk();
    ~chunk();

    void set_global_pos(Vector2 pos);

    int set_block(block b, Vector2 pos);

    int set_block_index(block_type* b, int index);

    block* get_block(Vector2 pos);
    
    block* get_block_index(int index);

    Vector2 get_chunk_pos();

    json serialize_chunk();

    int new_chunk_from_json(json j);


};

class world_gen;

class world_class {
    friend class world_gen;
    private:
    

        std::vector<chunk> chunks;

        #define MAX_TABLE_SIZE 128

        int pos_x_neg_y[MAX_TABLE_SIZE][MAX_TABLE_SIZE];
        int neg_x_neg_y[MAX_TABLE_SIZE][MAX_TABLE_SIZE];
        int neg_x_pos_y[MAX_TABLE_SIZE][MAX_TABLE_SIZE];
        int pos_x_pos_y[MAX_TABLE_SIZE][MAX_TABLE_SIZE];

        int look_up_chunk_index(Vector2 coord);

        int add_look_up(Vector2 coord, int index);

        bool is_chunk_index_valid(int index);

        chunk* get_chunk(Vector2 chunk_cord);

        int add_chunk(chunk* chnk);

        Vector2 get_chunk_coord(Vector2 real_coord);

        Vector2 get_sub_chunk_pos(Vector2 real_coord);


    public:

        int place_block(Vector2 pos, block b);

        block* get_block(Vector2 pos);
};

class world_gen{

};