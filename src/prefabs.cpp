#include "leap_internal.h"

std::unique_ptr<leap::Object> leap::prefabs::MakeMeshObject(const shapes::MeshData &mesh, const glm::vec3 &position, const glm::vec4 &color, const glm::vec3 &scale, std::shared_ptr<Texture> texture) {
    auto obj = std::make_unique<Object>();

    auto transform = obj->AddComponent<Transform>();
    transform->position = position;
    transform->scale = scale;

    obj->AddComponent<Mesh>(mesh);

    auto material = obj->AddComponent<Material>();
    material->color = color;
    material->texture = std::move(texture);
    
    return obj;
}

std::unique_ptr<leap::Object> leap::prefabs::MakePointLight(const glm::vec3 &position, const glm::vec3 &color, float intensity, float range) {
    auto obj = std::make_unique<Object>();

    auto transform = obj->AddComponent<Transform>();
    transform->position = position;

    auto light = obj->AddComponent<Light>();
    light->type = LightType::Point;
    light->color = color;
    light->intensity = intensity;
    light->range = range;

    return obj;
}

std::unique_ptr<leap::Object> leap::prefabs::MakeDirectionalLight(const glm::vec3 &direction, const glm::vec3 &color, float intensity) {
    auto obj = std::make_unique<Object>();

    auto light = obj->AddComponent<Light>();
    light->type = LightType::Directional;
    light->direction = direction;
    light->color = color;
    light->intensity = intensity;

    return obj;
}