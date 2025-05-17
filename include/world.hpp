#include "raylib.h"
#include "sphysics.h"
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <math.h>
#include "json.hpp"
#include "defines.hpp"
#include "rng.hpp"
#include "FastNoiseLite.h"
#include "item_master.hpp"
#include "block_master.hpp"

#pragma once

using json = nlohmann::json;
#define CHUNK_SIZE 16

class chunk{
    private:
        block* blocks;

        Vector2 global_pos;

        int chunk_id;

    public:

    chunk();
    ~chunk();

    //set its position
    void set_global_pos(Vector2 pos);

    //sets a block inside of itself (pass sub chunk coords (example a corner of a chunk it 0, 0))
    int set_block(block b, Vector2 pos);

    //same as befor but with an index and not a Vector2
    int set_block_index(block_type* b, int index);

    //gets the block
    block* get_block(Vector2 pos);

    //gets the block by index
    block* get_block_index(int index);

    //gets the chunk pos
    Vector2 get_chunk_pos();

    //serializes the chunk
    json serialize_chunk();

    //creates new chunk from json
    int new_chunk_from_json(json j);

    const block* blocks_buffer();
};
class world_class {
    private:
    

        std::vector<chunk> chunks;

        #define MAX_TABLE_SIZE 128

        int pos_x_neg_y[MAX_TABLE_SIZE][MAX_TABLE_SIZE];
        int neg_x_neg_y[MAX_TABLE_SIZE][MAX_TABLE_SIZE];
        int neg_x_pos_y[MAX_TABLE_SIZE][MAX_TABLE_SIZE];
        int pos_x_pos_y[MAX_TABLE_SIZE][MAX_TABLE_SIZE];

        //looks for the index of a chunk based on its coordinates using a table since a chunk's position in the chunks array does not determin its location
        int look_up_chunk_index(Vector2 coord);

        //adds an entry to the lookup table. coord is the cordinates and index is the array index the chunk is stored in.
        int add_look_up(Vector2 coord, int index);

        //checks to see if the chunk index is valid
        bool is_chunk_index_valid(int index);

        //grabs a raw chunk pointer
        chunk* get_chunk(Vector2 chunk_cord);

        //adds a chunk from a raw chunk pointer
        int add_chunk(chunk* chnk);

        //gets the chunk coordinate based on a block coordinate
        Vector2 get_chunk_coord(Vector2 real_coord);

        //gets the sub chunk coordinate from a global block coordiante
        Vector2 get_sub_chunk_pos(Vector2 real_coord);

        chunk* generate_chunk(Vector2 pos);


    public:
        world_class();

        //adds a new chunk from json
        int new_chunk_from_json(json j);

        //places a block in the world
        int place_block(Vector2 pos, block b);

        //gets a block in the world
        block* get_block(Vector2 pos);

        const block* chunk_buffer(Vector2 pos);
};

extern world_class world;