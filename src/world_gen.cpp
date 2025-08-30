#include "world.hpp"
#include "defines.hpp"
#include "rng.hpp"
#include "item_master.hpp"
#include "block_master.hpp"
#include "networking.hpp"

world_gen::world_gen(){
    caves.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    warmth.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    compression.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    seed = random_num.get_random_int();

    caves.SetFrequency(0.05f);
    caves.SetSeed(seed);

    ores = item_manager.get_all_ores();

}

void world_gen::sprinkle_ores(chunk* chnk, Vector2 pos){

    Vector2 real_pos;
    real_pos.x = pos.x*CHUNK_SIZE;
    real_pos.y = pos.y*CHUNK_SIZE;

    ores = item_manager.get_all_ores();

    int chance_to_spawn_vein = random_num.get_random_int_range(1, 2);

    if(chance_to_spawn_vein == 1 && compression.GetNoise(real_pos.x*CHUNK_SIZE, real_pos.y*CHUNK_SIZE) < 0.2){
        bool ore_spawned = false;
        int max_retry = ores.size();
        int inc = 0;

        int chance_size = 0;

        for(int i = 0; i < ores.size(); i++){
            chance_size = chance_size + (int)round(ores[i].chance_to_spawn);
            //std::cout << "\nchance to spawn: " << std::to_string(ores[i].chance_to_spawn) << std::endl;
        }

        //std::cout << "\nchance size = " << std::to_string(chance_size) << std::endl;

        while(ore_spawned == false){
            if(chance_size <= 0) break;

            int rngo = random_num.get_random_int_range(0, chance_size - 1);

            int chance_pointer = 0;

            for(int i = 0; i < ores.size(); i++){
                if(rngo > chance_pointer && rngo < ores[i].chance_to_spawn + chance_pointer){
                    ore_spawned = true;

                    Vector2 offset;
                    offset.x = random_num.get_random_float_range(0, CHUNK_SIZE);
                    offset.y = random_num.get_random_float_range(0, CHUNK_SIZE);

                    block blk;

                    blk.state = 0;
                    blk.attr = ores[i].block_type_ptr;

                    //world.set_block_radius(blk, ores[i].ore_radius, {real_pos.x + offset.x, real_pos.y + offset.y});

                    chnk->set_block(blk.attr, {CHUNK_SIZE/2, CHUNK_SIZE/2});
                    if(ores[i].ore_radius >= 2){
                        chnk->set_block(blk.attr, {(CHUNK_SIZE/2) + 1, (CHUNK_SIZE/2)});
                        chnk->set_block(blk.attr, {(CHUNK_SIZE/2) - 1, (CHUNK_SIZE/2)});

                        chnk->set_block(blk.attr, {(CHUNK_SIZE/2), (CHUNK_SIZE/2) + 1});
                        chnk->set_block(blk.attr, {(CHUNK_SIZE/2), (CHUNK_SIZE/2) - 1});
                    }

                    if(ores[i].ore_radius >= 3){
                        chnk->set_block(blk.attr, {(CHUNK_SIZE/2) + 2, (CHUNK_SIZE/2)});
                        chnk->set_block(blk.attr, {(CHUNK_SIZE/2) - 2, (CHUNK_SIZE/2)});

                        chnk->set_block(blk.attr, {(CHUNK_SIZE/2), (CHUNK_SIZE/2) + 2});
                        chnk->set_block(blk.attr, {(CHUNK_SIZE/2), (CHUNK_SIZE/2) - 2});

                        chnk->set_block(blk.attr, {(CHUNK_SIZE/2) + 1, (CHUNK_SIZE/2) + 1});
                        chnk->set_block(blk.attr, {(CHUNK_SIZE/2) - 1, (CHUNK_SIZE/2) + 1});

                        chnk->set_block(blk.attr, {(CHUNK_SIZE/2) + 1, (CHUNK_SIZE/2) - 1});
                        chnk->set_block(blk.attr, {(CHUNK_SIZE/2) - 1, (CHUNK_SIZE/2) - 1});
                    }

                    //std::cout << "spawned ore\n";

                    break;
                }
                chance_pointer = chance_pointer + ores[i].chance_to_spawn;

            }
        }
    } 
}

void world_gen::generate_mass(chunk* chnk, Vector2 pos){
    //fill stone
    for (int x  = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            chnk->set_block(item_manager.fetch_item("blackstone")->block_type_ptr, {(float)x, (float)y});
        }
    }



    for (int x  = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            float n = compression.GetNoise(((float)x + (pos.x*CHUNK_SIZE)), ((float)y + (pos.y*CHUNK_SIZE)));
            float n2 = warmth.GetNoise(((float)x + (pos.x*CHUNK_SIZE)), ((float)y + (pos.y*CHUNK_SIZE)));

            n2 = abs(n2);

            if (n >= 0 && n < 0.2) {
                chnk->set_block(item_manager.fetch_item("stone")->block_type_ptr, {(float)x, (float)y});
            }

            if(n >= 0.2){
                chnk->set_block(item_manager.fetch_item("gravel")->block_type_ptr, {(float)x, (float)y});
                //std::cout << std::to_string(n2) << std::endl;
                if(n2 > 0.5 && n2 <= 0.7){
                    chnk->set_block(item_manager.fetch_item("dirt")->block_type_ptr, {(float)x, (float)y});
                }

                if(n2 > 0.95){
                    chnk->set_block(item_manager.fetch_item("lava")->block_type_ptr, {(float)x, (float)y});
                }
            }
        }
    }
}

void world_gen::generate_caves(chunk* chnk, Vector2 pos){
    for (int x  = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            float n = caves.GetNoise(((float)x + (pos.x*CHUNK_SIZE)), ((float)y + (pos.y*CHUNK_SIZE)));

            if (n >= 0.5) {
                chnk->set_block(item_manager.fetch_item("air")->block_type_ptr, {(float)x, (float)y});
            }
        }
    }
}

chunk* world_gen::generate_chunk(Vector2 pos){
    chunk* chnk = new chunk;

    chnk->set_global_pos(pos);

    generate_mass(chnk, pos);

    sprinkle_ores(chnk, pos);

    generate_caves(chnk, pos);

    for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++) {
        block_type* b;
        b = block_manager.fetch_block_type(item_manager.fetch_item("stone")->block_type_ptr->type);
        chnk->set_block_index(b, i, true);
    }

    return chnk;
}