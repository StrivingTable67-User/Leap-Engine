#include "leap_internal.h"

int leap::Random::GetRandomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(s_engine);
}

float leap::Random::GetRandomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(s_engine);
}

bool leap::Random::GetRandomBool(double chanceForTrue) {
    std::bernoulli_distribution dist(chanceForTrue);
    return dist(s_engine);
}

void leap::Random::SetSeed(unsigned int seed) {
    s_engine.seed(seed);
}