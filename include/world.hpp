#pragma once
#include "raylib.h"
//#include "sphysics.h"
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <math.h>
#include <climits>
#include "json.hpp"
#include "rng.hpp"
#include "FastNoiseLite.h"

class block;
struct block_type;
struct item;



#pragma once

using json = nlohmann::json;
#define CHUNK_SIZE 16
#define BLOCK_SIZE 32
#define NOISE_SCALE 1

class chunk{
    private:

        std::vector<block> l1;
        std::vector<block> l2;

        Vector2 global_pos;

        int chunk_id;

    public:

    chunk();

    //set its position
    void set_global_pos(Vector2 pos);

    //sets a block inside of itself (pass sub chunk coords (example a corner of a chunk it 0, 0))
    int set_block(block_type* b, Vector2 pos, bool bg = false);

    //same as befor but with an index and not a Vector2
    int set_block_index(block_type* b, int index, bool bg = false);

    //gets the block
    block* get_block(Vector2 pos, bool bg = false);

    //gets the block by index
    block* get_block_index(int index, bool bg = false);

    //gets the chunk pos
    Vector2 get_chunk_pos();

    //serializes the chunk
    json serialize_chunk();


    //creates new chunk from json
    int new_chunk_from_json(json j);

    const block* blocks_buffer(bool bg = false) const;
};



class world_gen{
    private:

        std::vector<item> ores;

        int seed;
        rng_device rng_ore;
        FastNoiseLite caves;
        FastNoiseLite warmth;
        FastNoiseLite compression;

        void generate_mass(chunk* chnk, Vector2 pos);

        void sprinkle_ores(chunk* chnk, Vector2 pos);

        void generate_caves(chunk* chnk, Vector2 pos);

        //void generate_liquids(chunk* chnk, Vector2 pos);

    public:

        world_gen();

        chunk* generate_chunk(Vector2 pos);


};

class save_master;

class world_class {
    friend class network;
    friend class client;
    friend class server;

    friend class save_master;
    private:
    
        std::vector<chunk> chunks;

        world_gen generator;

        #define MAX_TABLE_SIZE 2560

        int** pos_x_neg_y;
        int** neg_x_neg_y;
        int** neg_x_pos_y;
        int** pos_x_pos_y;

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

        const block* chunk_buffer(Vector2 pos, bool bg = false);

        json serialize_chunk(Vector2 pos);

        int load_chunk(json j);

        void set_block_radius(block blk, int height, Vector2 pos);

        void break_block(Vector2 pos);
};

extern world_class world;
