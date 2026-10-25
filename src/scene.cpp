#include "leap_internal.h"
#include <algorithm>
#include <cmath>

static float WorldRadius(const leap::Transform *t, const leap::Collider *c) {
    return c->radius * std::max({ std::abs(t->scale.x), std::abs(t->scale.y), std::abs(t->scale.z) });
}

static void WorldBounds(const leap::Transform *t, const leap::Collider *c, glm::vec3 &outMin, glm::vec3 &outMax) {
    glm::vec3 half = c->halfExtents * glm::abs(t->scale);
    outMin = t->position - half;
    outMax = t->position + half;
}

static bool CollidersOverlap(const leap::Transform *ta, const leap::Collider *ca,
                             const leap::Transform *tb, const leap::Collider *cb) {
    using leap::ColliderType;

    if (ca->type == ColliderType::Sphere && cb->type == ColliderType::Sphere) {
        return glm::distance(ta->position, tb->position) < WorldRadius(ta, ca) + WorldRadius(tb, cb);
    }

    if (ca->type == ColliderType::Box && cb->type == ColliderType::Box) {
        glm::vec3 minA, maxA, minB, maxB;
        WorldBounds(ta, ca, minA, maxA);
        WorldBounds(tb, cb, minB, maxB);
        return minA.x <= maxB.x && maxA.x >= minB.x &&
               minA.y <= maxB.y && maxA.y >= minB.y &&
               minA.z <= maxB.z && maxA.z >= minB.z;
    }

    bool aIsSphere = (ca->type == ColliderType::Sphere);
    const leap::Transform *st = aIsSphere ? ta : tb;
    const leap::Collider  *sc = aIsSphere ? ca : cb;
    const leap::Transform *bt = aIsSphere ? tb : ta;
    const leap::Collider  *bc = aIsSphere ? cb : ca;

    glm::vec3 boxMin, boxMax;
    WorldBounds(bt, bc, boxMin, boxMax);
    glm::vec3 closest = glm::clamp(st->position, boxMin, boxMax);
    return glm::distance(st->position, closest) < WorldRadius(st, sc);
}

void leap::Scene::AddObject(const std::string &name, std::unique_ptr<Object> object) {
    objects[name] = std::move(object);
}

void leap::Scene::RemoveObject(const std::string &name) {
    objects.erase(name);
}

leap::Object *leap::Scene::FindObject(const std::string &name) {
    auto it = objects.find(name);
    return it != objects.end() ? it->second.get() : nullptr;
}

void leap::Scene::Update(double deltaTime) {
    for (auto &[type, obj] : objects) {
        if (obj) {
            obj->Update(deltaTime);
        }
    }
}

void leap::Scene::Draw() {
    for (auto &[type, obj] : objects) {
        if (obj) {
            obj->Draw();
        }
    }
}

size_t leap::Scene::Count() const {
    return objects.size();
}

std::vector<leap::Object *> leap::Scene::FindObjectsWithTag(const std::string &tag) {
    std::vector<Object *> result = {};
    for (auto &[name, obj] : objects) {
        if (obj && obj->HasTag(tag)) {
            result.push_back(obj.get());
        }
    }

    return result;
}

std::vector<leap::CollisionPair> leap::Scene::GetOverlaps() {
    std::vector<CollisionPair> result;

    std::vector<Object *> candidates;
    for (auto &[name, obj] : objects) {
        if (obj && obj->GetComponent<Collider>() && obj->GetComponent<Transform>()) {
            candidates.push_back(obj.get());
        }
    }

    for (size_t i = 0; i < candidates.size(); ++i) {
        for (size_t j = i + 1; j < candidates.size(); ++j) {
            Object *a = candidates[i];
            Object *b = candidates[j];
            if (CollidersOverlap(a->GetComponent<Transform>(), a->GetComponent<Collider>(),
                                 b->GetComponent<Transform>(), b->GetComponent<Collider>())) {
                result.push_back({a, b});
            }
        }
    }

    return result;
}