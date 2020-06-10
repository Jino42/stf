#include <Engine/Random.hpp>
#include "Random.hpp"


std::unique_ptr<Random> Random::instance_ = nullptr;

float Random::getRandom01() {
	std::uniform_real_distribution<float> distribution(0.f, 1.f);

	std::mt19937_64 rng(rd_());

	return distribution(rng);
}
Random::Random()
        :
        seed_(0, std::numeric_limits<int>::max()) {
}
