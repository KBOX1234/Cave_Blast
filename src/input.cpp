#include "input.hpp"

int kb(key_bind kbind) {
    if (IsKeyDown(kbind.custom_key)) {
        return kbind.org_key;
    }
    else return 0;
}

void input::update_movement() {
    if (IsKeyDown(KEY_W)) player_manager.host->move_player_y(-1);
    if (IsKeyDown(KEY_S)) player_manager.host->move_player_y(1);
    if (IsKeyDown(KEY_D)) player_manager.host->move_player_x(1);
    if (IsKeyDown(KEY_A)) player_manager.host->move_player_x(-1);
}


void input::update() {
    update_movement();
}
