#include <chrono>
#include <math.h>

#define micro_second std::chrono::system_clock::time_point

class delta_time {
private:
    bool can_tick_happen;

    micro_second start;

    micro_second last_tick;

    micro_second current_tick;

    int tick_speed = 60;

    int get_milliseconds_per_tick();


public:

    delta_time();

    bool can_game_continue();

    void update();


};

extern delta_time delta_time_master;