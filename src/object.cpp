#include "leap_internal.h"

leap::Object::~Object() {
    for (auto &[type, comp] : components) { comp->Cleanup(); }
    components.clear();
}

void leap::Object::Update(double deltaTime) {
    for (auto &[type, comp] : components) { comp->Update(deltaTime); }
}

void leap::Object::Draw() {
    for (auto &[type, comp] : components) { comp->Draw(); }
}

void leap::Object::AddTag(const std::string &tag) {
    tags.insert(tag);
}

bool leap::Object::HasTag(const std::string &tag) const {
    return tags.find(tag) != tags.end();
}

void leap::Object::RemoveTag(const std::string &tag) {
    tags.erase(tag);
}