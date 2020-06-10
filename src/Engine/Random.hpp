#pragma once
#include <random>
#include <memory>
#include <limits>

class Random {
public:

    Random();

    int getRandomSeed()
    {
        std::mt19937 rng(rd_());
        return seed_(rng);
    }

    float getRandom01();

    static Random &Get() {
        if (!instance_) {
            instance_ = std::make_unique<Random>();
        }
        return *Random::instance_;
    }

private:
    std::random_device rd_;
    std::uniform_int_distribution<int> seed_;

    static std::unique_ptr<Random> instance_;
};