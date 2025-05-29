#include "rng.hpp"

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

int rng_device::get_random_int_range(int a, int b){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(a, b);

    int random_number = dist(gen);

    return random_number;
}
float rng_device::get_random_float_range(float a, float b) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(a, b); // Range: [a, b)

    float random_number = dist(gen);
    return random_number;
}
