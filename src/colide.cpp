#include "colide.hpp"

point colide::v2p(Vector2 v){
    point rtv;

    rtv.x = v.x;
    rtv.y = v.y;

    return rtv;
}

Vector2 colide::p2v(point p){
    Vector2 rtv;

    rtv.x = p.x;
    rtv.y = p.y;

    return rtv;
}

void colide::set_player_base(player_master* pb){
    player_base = pb;
}

void colide::set_world_base(world_class* wb){
    world_base = wb;
}

#define CHECK_BLOCKS_X 3
#define CHECK_BLOCKS_Y 4

bool colide::check_player(player* p){
    p->update_colide_box();



    const size_t size = CHECK_BLOCKS_X*CHECK_BLOCKS_Y;

    Vector2 pointer_p = p->get_block_pos();

    pointer_p.x--;
    pointer_p.y--;

    block* check_block;


    for(int i = 0; i < size; i++){
        check_block = world_base->get_block(pointer_p);
        //std::cout << "pos: " + std::to_string(pointer_p.x) + ", " + std::to_string(pointer_p.y) + "\n";
        if(check_block != nullptr){
            colideBox tmp_box;

            tmp_box.a = v2p(pointer_p);
            tmp_box.b.x = (pointer_p.x*BLOCK_SIZE) + BLOCK_SIZE;
            tmp_box.b.y = (pointer_p.y*BLOCK_SIZE) + BLOCK_SIZE;

            if(doesBoxAndBoxColide(&p->box, &tmp_box) && item_manager.fetch_item_by_id(check_block->attr->item_id)->name != "air"){

                float rotation = p->get_rotation() - (360/2);

                p->zero_rotation();

                p->increase_angle(rotation);

                p->move_player();

                std::cout << "colide\n";

                DrawRectangleLines(pointer_p.x, pointer_p.y, BLOCK_SIZE, BLOCK_SIZE, RED);
            }
            
            else{
                DrawRectangleLines(pointer_p.x, pointer_p.y, BLOCK_SIZE, BLOCK_SIZE, GREEN);
            }
        } 


        pointer_p.x++;
        if(pointer_p.x > CHECK_BLOCKS_X + (p->get_block_pos().x - 1)){
            pointer_p.x = p->get_block_pos().x - 1;
            pointer_p.y++;
        }
    }

    return true;

}

void colide::check_player_base(){
    check_player(player_base->myself);
    for(int i = 0; i < player_base->players.size(); i++){
        check_player(player_base->players[i]);
    }
}