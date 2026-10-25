#include "leap_internal.h"
#include <cmath>
#include <algorithm>

leap::Tween *leap::Tween::Position(leap::Transform *target, glm::vec3 to, float duration, leap::EaseType ease) {
    auto tween = std::make_unique<Tween>();
    tween->target = target;
    tween->field = Field::Position;
    tween->from = target->position;
    tween->to = to;
    tween->duration = duration;
    tween->ease = ease;

    Tween *ptr = tween.get();
    activeTweens.push_back(std::move(tween));
    return ptr;
}

leap::Tween *leap::Tween::Rotation(leap::Transform *target, glm::vec3 to, float duration, leap::EaseType ease) {
    auto tween = std::make_unique<Tween>();
    tween->target = target;
    tween->field = Field::Rotation;
    tween->from = target->rotation;
    tween->to = to;
    tween->duration = duration;
    tween->ease = ease;

    Tween *ptr = tween.get();
    activeTweens.push_back(std::move(tween));
    return ptr;
}

leap::Tween *leap::Tween::Scale(leap::Transform *target, glm::vec3 to, float duration, leap::EaseType ease) {
    auto tween = std::make_unique<Tween>();
    tween->target = target;
    tween->field = Field::Scale;
    tween->from = target->scale;
    tween->to = to;
    tween->duration = duration;
    tween->ease = ease;

    Tween *ptr = tween.get();
    activeTweens.push_back(std::move(tween));
    return ptr;
}

void leap::Tween::UpdateAll(double deltaTime) {
    for (auto &tween : activeTweens) {
        if (!tween->finished && !tween->cancelled) {
            tween->Update(deltaTime);
        }
    }

    activeTweens.erase(
        std::remove_if(activeTweens.begin(), activeTweens.end(),
            [](const std::unique_ptr<Tween> &t) { return t->finished || t->cancelled; }),
        activeTweens.end()
    );
}

void leap::Tween::Cancel() {
    cancelled = true;
}

bool leap::Tween::IsFinished() const {
    return finished;
}

void leap::Tween::Update(double deltaTime) {
    elapsed += static_cast<float> (deltaTime);
    float t = glm::clamp(elapsed / duration, 0.0f, 1.0f);
    float easedT = ApplyEase(t, ease);
    glm::vec3 value = glm::mix(from, to, easedT);

    switch (field) {
        case Field::Position: target->position = value; break;
        case Field::Rotation: target->rotation = value; break;
        case Field::Scale:    target->scale    = value; break;
    }

    if (t >= 1.0f) { finished = true; }
}

float leap::Tween::ApplyEase(float t, leap::EaseType ease) {
    switch (ease) {
        case EaseType::EaseIn:    return t * t;
        case EaseType::EaseOut:   return 1.0f - (1.0f - t) * (1.0f - t);
        case EaseType::EaseInOut: return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
        default:                  return t;
    }
}