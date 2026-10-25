#include "leap_internal.h"

leap::Input::Input(GLFWwindow *window) {
    s_window = window;
    s_lastMousePos = glm::vec2(0.0f);
    s_mouseDelta = glm::vec2(0.0f);
    s_firstMouse = true;
}

void leap::Input::Update() {
    glfwPollEvents();

    double x, y;
    glfwGetCursorPos(s_window, &x, &y);
    glm::vec2 current(static_cast<float> (x), static_cast<float> (y));

    if (s_firstMouse) {
        s_lastMousePos = current;
        s_firstMouse = false;
    }

    s_mouseDelta = current - s_lastMousePos;
    s_lastMousePos = current;
}

static_assert(static_cast<int> (leap::Key::W)           == GLFW_KEY_W,            "leap::Key out of sync with GLFW");
static_assert(static_cast<int> (leap::Key::Space)       == GLFW_KEY_SPACE,        "leap::Key out of sync with GLFW");
static_assert(static_cast<int> (leap::Key::LeftControl) == GLFW_KEY_LEFT_CONTROL, "leap::Key out of sync with GLFW");
static_assert(static_cast<int> (leap::Key::F12)         == GLFW_KEY_F12,          "leap::Key out of sync with GLFW");

bool leap::Input::IsKeyDown(Key key) {
    return glfwGetKey(s_window, static_cast<int> (key)) == GLFW_PRESS;
}

glm::vec2 leap::Input::GetMouseDelta() {
    return s_mouseDelta;
}

void leap::Input::SetCursorLocked(bool locked) {
    glfwFocusWindow(s_window);
    glfwSetInputMode(s_window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    if (locked) {
        s_firstMouse = true;
        if (glfwRawMouseMotionSupported()) {
            glfwSetInputMode(s_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
    }
}