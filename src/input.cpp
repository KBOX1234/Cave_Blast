#include "input.hpp"
#include "world.hpp"
#include "player.hpp"
#include "imgui_window.hpp"
#include "networking.hpp"
#include "item_convert.hpp"


int kb(key_bind kbind) {
    if (IsKeyDown(kbind.custom_key)) {
        return kbind.org_key;
    }
    else return 0;
}

void input::update_movement() {
    if (IsKeyDown(keybind_manager.move_upy_key)) {

        player_manager.myself->zero_rotation();
        player_manager.myself->increase_angle(270);

        player_manager.myself->move_player();

        is_inp_update = true;
    }
    if (IsKeyDown(keybind_manager.move_downy_key)) {
        player_manager.myself->zero_rotation();
        player_manager.myself->increase_angle(90);

        player_manager.myself->move_player();

        is_inp_update = true;

    }
    if (IsKeyDown(keybind_manager.move_righty_key)) {
        player_manager.myself->zero_rotation();
        player_manager.myself->increase_angle(0);

        player_manager.myself->move_player();

        is_inp_update = true;
    }
    if (IsKeyDown(keybind_manager.move_lefty_key)) {
        player_manager.myself->zero_rotation();
        player_manager.myself->increase_angle(180);

        player_manager.myself->move_player();

        is_inp_update = true;
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

    if (IsKeyPressed(keybind_manager.break_block_key)) player_manager.myself->break_block({cursor.x - 1, cursor.y});

    if (IsKeyPressed(keybind_manager.place_block_key)) player_manager.myself->place_block({cursor.x - 1, cursor.y});

    if (IsKeyPressed(keybind_manager.move_cursor_lefty_key)) {
        cursor_offset.x--;
    }

    if (IsKeyPressed(keybind_manager.move_cursor_righty_key)) {
        cursor_offset.x++;
    }

    if (IsKeyPressed(keybind_manager.move_cursor_upy_key)) {
        cursor_offset.y--;
    }

    if (IsKeyPressed(keybind_manager.move_cursor_downy_key)) {
        cursor_offset.y++;
    }

    //std::cout << "cursor pos: " << std::to_string(cursor.x) << ", " << std::to_string(cursor.y) << std::endl;
}

void input::craft_convert_controles() {
    if (IsKeyPressed(keybind_manager.convert_item)) {
        block* blk = world.get_block({cursor.x - 1, cursor.y});

        item itm = player_manager.inv_ui.current_item->item_i;

        item_convert_manager.convert_item(&itm, *blk, player_manager.myself);

    }
}


void input::update() {
    update_movement();
    mine_controles();
    craft_convert_controles();
    player_manager.inv_ui.update_inv_ui_input();
}
