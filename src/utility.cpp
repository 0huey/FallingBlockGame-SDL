#include "utility.hpp"
#include <random>
#include <iostream>

static bool g_srand_called = false;
static std::random_device rand_device;
static std::mt19937_64 rand_engine;
static std::uniform_int_distribution<Uint64> rand_distribution;

uint64_t Random()
{
    if (!g_srand_called)
    {
        rand_engine = std::mt19937_64(rand_device());
        rand_distribution = std::uniform_int_distribution<uint64_t>(0, (uint64_t)std::pow(2,63));
        g_srand_called = true;
    }

    return rand_distribution(rand_engine);
}

uint64_t Random(uint64_t min, uint64_t max)
{
    auto rand = std::mt19937_64(rand_device());
    auto dist = std::uniform_int_distribution<uint64_t>(0, max);
    return dist(rand);
}