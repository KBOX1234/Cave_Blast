#pragma once
#include <unordered_set>
#include <random>

class rng_device{
    private:
        std::random_device rd;

        std::unordered_set<int> history;

        bool is_unique(int number);


    public:

        int get_random_int();

        int get_random_int_range(int a, int b);
        float get_random_float_range(float a, float b);
};
#pragma once

extern rng_device random_num;
