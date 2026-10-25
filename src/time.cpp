#include "leap_internal.h"

leap::Time::Time() {
    lastTime = glfwGetTime();
    deltaTime = 0.0f;
}

double leap::Time::GetDeltaTime() {
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    return deltaTime;
}

double leap::Time::GetDeltaTimeMs() {
    return GetDeltaTime() * 1000;
}