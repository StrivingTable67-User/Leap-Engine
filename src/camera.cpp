#include "leap_internal.h"
#include <cmath>

leap::Camera::Camera(float aspectRatio) {
    m_aspectRatio = aspectRatio;
}

glm::vec3 leap::Camera::GetForward() const {
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return glm::normalize(f);
}

glm::vec3 leap::Camera::GetRight() const {
    return glm::normalize(glm::cross(GetForward(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

glm::mat4 leap::Camera::GetViewMatrix() const {
    return glm::lookAt(position, position + GetForward(), up);
}

glm::mat4 leap::Camera::GetProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), m_aspectRatio, nearPlane, farPlane);
}

void leap::Camera::SetMode(CameraMode newMode) {
    m_mode = newMode;
    mainInput->SetCursorLocked(m_mode != CameraMode::None);
}

void leap::Camera::Update(double deltaTime) {
    switch (m_mode) {
        case CameraMode::FPS:   UpdateFPS(deltaTime);   break;
        case CameraMode::Space: UpdateSpace(deltaTime); break;
        case CameraMode::None:  break;
    }
}

void leap::Camera::UpdateFPS(double deltaTime) {
    glm::vec2 delta = mainInput->GetMouseDelta();
    yaw += delta.x * mouseSensitivity;
    pitch = glm::clamp(pitch - delta.y * mouseSensitivity, -89.0f, 89.0f);

    glm::vec3 forward = glm::normalize(glm::vec3(GetForward().x, 0.0f, GetForward().z));
    glm::vec3 right = GetRight();
    float v = moveSpeed * static_cast<float> (deltaTime);

    if (mainInput->IsKeyDown(Key::W)) { position += forward * v; }
    if (mainInput->IsKeyDown(Key::S)) { position -= forward * v; }
    if (mainInput->IsKeyDown(Key::D)) { position += right   * v; }
    if (mainInput->IsKeyDown(Key::A)) { position -= right   * v; }
}

void leap::Camera::UpdateSpace(double deltaTime) {
    glm::vec2 delta = mainInput->GetMouseDelta();
    yaw += delta.x * mouseSensitivity;
    pitch = glm::clamp(pitch - delta.y * mouseSensitivity, -89.0f, 89.0f);

    glm::vec3 forward = GetForward();
    glm::vec3 right   = GetRight();
    float v = moveSpeed * static_cast<float> (deltaTime);

    if (mainInput->IsKeyDown(Key::W))           { position += forward * v; }
    if (mainInput->IsKeyDown(Key::S))           { position -= forward * v; }
    if (mainInput->IsKeyDown(Key::D))           { position += right   * v; }
    if (mainInput->IsKeyDown(Key::A))           { position -= right   * v; }
    if (mainInput->IsKeyDown(Key::Space))       { position += up      * v; }
    if (mainInput->IsKeyDown(Key::LeftControl)) { position -= up      * v; }
}