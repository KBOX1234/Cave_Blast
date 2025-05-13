#include "rng.hpp"
rng_device random_num;

bool rng_device::is_unique(int number) {
    return history.find(number) == history.end();
}

int rng_device::get_random_int(){
    int random_num = rd();

    while(is_unique(random_num) == false){
        random_num = rd();
    }

    history.insert(random_num);

    return random_num;
}