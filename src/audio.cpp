#define MINIAUDIO_IMPLEMENTATION
#include "leap_internal.h"
#include <miniaudio.h>
#include <iostream>

struct leap::AudioEngine::Impl {
    ma_engine engine;
};

struct leap::AudioSource::Impl {
    ma_sound sound;
    bool loaded = false;
};

leap::AudioEngine::AudioEngine() : impl(std::make_unique<Impl>()) {
    if (ma_engine_init(NULL, &impl->engine) != MA_SUCCESS) {
        throw Error("Failed to Initialize Audio Engine");
    }
}

leap::AudioEngine::~AudioEngine() {
    ma_engine_uninit(&impl->engine);
}

void leap::AudioEngine::SetListenerPosition(const glm::vec3 &position) {
    ma_engine_listener_set_position(&impl->engine, 0, position.x, position.y, position.z);
}

void leap::AudioEngine::SetListenerDirection(const glm::vec3 &forward, const glm::vec3 &up) {
    ma_engine_listener_set_direction(&impl->engine, 0, forward.x, forward.y, forward.z);
    ma_engine_listener_set_world_up(&impl->engine, 0, up.x, up.y, up.z);
}

leap::AudioSource::AudioSource(const std::string &path) : impl(std::make_unique<Impl>()) {
    if (!mainAudioEngine) {
        std::cout << "ERROR: AudioSource created before SetDefaultAudio()\n";
        return;
    }

    ma_result result = ma_sound_init_from_file(&mainAudioEngine->impl->engine, path.c_str(), 0, NULL, NULL, &impl->sound);
    if (result != MA_SUCCESS) {
        std::cout << "ERROR: Failed to Load Sound \"" << path << "\"\n";
        return;
    }
    impl->loaded = true;
}

leap::AudioSource::~AudioSource() {
    if (impl->loaded) { ma_sound_uninit(&impl->sound); }
}

void leap::AudioSource::Play() {
    if (!impl->loaded) { return; }
    ma_sound_set_volume(&impl->sound, volume);
    ma_sound_start(&impl->sound);
}

void leap::AudioSource::Stop() {
    if (!impl->loaded) { return; }
    ma_sound_stop(&impl->sound);
}

void leap::AudioSource::SetLooping(bool loop) {
    if (!impl->loaded) { return; }
    ma_sound_set_looping(&impl->sound, loop);
}

void leap::AudioSource::Update(double deltaTime) {
    if (!impl->loaded || !is3D) { return; }
    Transform *transform = owner->GetComponent<Transform>();
    if (transform) {
        ma_sound_set_position(&impl->sound, transform->position.x, transform->position.y, transform->position.z);
    }
}