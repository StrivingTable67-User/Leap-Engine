#include "leap_internal.h"
#include <algorithm>

void leap::Light::Init() {
    allLights.push_back(this);
}

void leap::Light::Cleanup() {
    allLights.erase(std::remove(allLights.begin(), allLights.end(), this), allLights.end());
}