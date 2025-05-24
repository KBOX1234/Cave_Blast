#include "../include/input.hpp"


int kb(key_bind kbind) {
    if (IsKeyDown(kbind.custom_key)) {
        return kbind.org_key;
    }
    else return 0;
}

void input::update_movement() {
    if (IsKeyDown(KEY_W)) {
        player_manager.get_host()->move_player();

        is_inp_update = true;
    }
    if (IsKeyDown(KEY_S)) {

        //is_inp_update = true;
    }
    if (IsKeyDown(KEY_D)) {
        player_manager.get_host()->increase_angle(1);
        //is_inp_update = true;
    }
    if (IsKeyDown(KEY_A)) {
        player_manager.get_host()->decrease(1);
        //is_inp_update = true;
    }

}

bool input::is_there_input_update() {
    if (is_inp_update == true) {
        is_inp_update = false;

        return true;
    }

    return false;
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

        if (networking.is_host() == true) {
            world.place_block({cursor.x - 1, cursor.y}, new_block);
        }

        client_utls::place_block("dirt", {cursor.x - 1, cursor.y});
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
