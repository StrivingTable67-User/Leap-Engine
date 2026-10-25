#include "leap_internal.h"

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

leap::Window::Window(unsigned int width, unsigned int height, const char *title, bool isFullscreen) {
    if (!glfwInit()) {
        throw Error("Failed to Initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    if (isFullscreen) {
        GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(primaryMonitor);
        m_window = glfwCreateWindow(mode->width, mode->height, title, primaryMonitor, NULL);
    } else {
        m_window = glfwCreateWindow(width, height, title, NULL, NULL);
    }

    if (m_window == NULL) {
        glfwTerminate();
        throw Error("Failed to Create GLFW Window");
    }
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        throw Error("Failed to Initialize GLAD");
    }

    int fbWidth = 0;
    int fbHeight = 0;
    glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);
    if (fbHeight > 0) { aspectRatio = static_cast<float> (fbWidth) / static_cast<float> (fbHeight); }

    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    
    glEnable(GL_DEPTH_TEST);
}

leap::Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void leap::Window::Render() {
    glfwSwapBuffers(m_window);
}

void leap::Window::ClearBackground(float r, float g, float b) {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void leap::Window::Close() {
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

float leap::Window::GetAspectRatio() const {
    return aspectRatio;
}

GLFWwindow *leap::Window::GetNativeWindow() const {
    return m_window;
}

bool leap::Window::ShouldClose() const {
    return glfwWindowShouldClose(m_window);
}