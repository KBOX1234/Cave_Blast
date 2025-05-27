#include "../include/delta_time.hpp"

delta_time::delta_time() {
    start = std::chrono::system_clock::now();

    last_tick = std::chrono::system_clock::now();

    current_tick = std::chrono::system_clock::now();
}


bool delta_time::can_game_continue() {
    return can_tick_happen;
}

int delta_time::get_milliseconds_per_tick() {
    return (int)round(1000.0f/(float)tick_speed);
}


void delta_time::update() {
    current_tick = std::chrono::system_clock::now();

    if (std::chrono::duration_cast<std::chrono::milliseconds>(current_tick - last_tick).count() > get_milliseconds_per_tick()) {
        last_tick = current_tick;
        can_tick_happen = true;
    }
    else {
        can_tick_happen = false;
    }

}
int delta_time::get_ticks_per_second() {
    return tick_speed;
}

