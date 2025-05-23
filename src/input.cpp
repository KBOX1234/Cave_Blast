#include "../include/input.hpp"


int kb(key_bind kbind) {
    if (IsKeyDown(kbind.custom_key)) {
        return kbind.org_key;
    }
    else return 0;
}

void input::update_movement() {
    if (IsKeyDown(KEY_W)) player_manager.get_host()->move_player_y(-1);
    if (IsKeyDown(KEY_S)) player_manager.get_host()->move_player_y(1);
    if (IsKeyDown(KEY_D)) player_manager.get_host()->move_player_x(1);
    if (IsKeyDown(KEY_A)) player_manager.get_host()->move_player_x(-1);
}

void input::mine_controles() {
    cursor.x = round(player_manager.get_host()->get_pos().x / BLOCK_SIZE) + cursor_offset.x;
    cursor.y = round(player_manager.get_host()->get_pos().y / BLOCK_SIZE) + cursor_offset.y;

    if (IsKeyPressed(KEY_ENTER)) {

        block new_block;
        new_block.state = 0;

        new_block.attr = item_manager.fetch_item("dirt")->block_type_ptr;

        //the coord should just be able to be directly passed but some logic error makes the block be placed 1 to the side
        //PLS FIX

        world.place_block({cursor.x - 1, cursor.y}, new_block);
    }

    if (IsKeyPressed(KEY_LEFT)) {
        cursor_offset.x--;
    }

    if (IsKeyPressed(KEY_RIGHT)) {
        cursor_offset.x++;
    }

    if (IsKeyPressed(KEY_UP)) {
        cursor_offset.y--;
    }

    if (IsKeyPressed(KEY_DOWN)) {
        cursor_offset.y++;
    }

    //std::cout << "cursor pos: " << std::to_string(cursor.x) << ", " << std::to_string(cursor.y) << std::endl;
}


void input::update() {
    update_movement();
    mine_controles();
}
