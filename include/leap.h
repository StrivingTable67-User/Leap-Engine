#ifndef LEAP_H
#define LEAP_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <cstdint>
#include <random>
#include <functional>
#include <stdexcept>

struct GLFWwindow;

namespace leap {
    using glm::vec2;
    using glm::vec3;
    using glm::vec4;
    using glm::mat2;
    using glm::mat3;
    using glm::mat4;

    class Window;
    class Input;
    class Shader;
    class AudioEngine;
    class Object;
    class Scene;
    class Component;
    class Texture;
    class Camera;
    class Light;

    inline Input *mainInput = nullptr;
    inline Shader *mainShader = nullptr;
    inline Camera *mainCam = nullptr;
    inline AudioEngine *mainAudioEngine = nullptr;
    inline std::vector<Light *> allLights = {};

    inline void SetDefaultInput(Input *input) {
        mainInput = input;
    }

    inline void SetDefaultShader(Shader *shader) {
        mainShader = shader;
    }

    inline void SetDefaultCamera(Camera *camera) {
        mainCam = camera;
    }

    inline void SetDefaultAudio(AudioEngine *engine) {
        mainAudioEngine = engine;
    }

    namespace shapes {
        struct MeshData {
            std::vector<float> vertices = {};
            std::vector<unsigned int> indices = {};
        };

        MeshData LoadCube();
        MeshData LoadPlane();
        MeshData LoadSphere(unsigned int stacks = 16, unsigned int slices = 16);
        MeshData LoadPyramid(unsigned int sides = 4);
        MeshData LoadCylinder(unsigned int stacks = 16, unsigned int slices = 16);

        MeshData LoadQuad();
        MeshData LoadPolygon(unsigned int sides = 90);

        MeshData LoadOBJ(const std::string &path);
    }

    namespace prefabs {
        std::unique_ptr<Object> MakeMeshObject(const shapes::MeshData &mesh, const glm::vec3 &position = glm::vec3(0.0f), const glm::vec4 &color = glm::vec4(1.0f), const glm::vec3 &scale = glm::vec3(1.0f), std::shared_ptr<Texture> texture = nullptr);
        std::unique_ptr<Object> MakePointLight(const glm::vec3 &position, const glm::vec3 &color = glm::vec3(1.0f), float intensity = 1.0f, float range = 10.0f);
        std::unique_ptr<Object> MakeDirectionalLight(const glm::vec3 &direction, const glm::vec3 &color = glm::vec3(1.0f), float intensity = 1.0f);
    }

    class Window {
        public:
            Window(unsigned int width = 1920, unsigned int height = 1080, const char *title = "Leap Engine (OpenGL 3.3 Core)", bool isFullscreen = true);
            ~Window();

            void Render();
            void ClearBackground(float r = 0.0f, float g = 0.0f, float b = 0.0f);

            void Close();

            float GetAspectRatio() const;
            GLFWwindow *GetNativeWindow() const;
            bool ShouldClose() const;
        private:
            float aspectRatio = 16.0f / 9.0f;
            GLFWwindow *m_window;
    };

    class Time {
        public:
            Time();
        
            double GetDeltaTime();
            double GetDeltaTimeMs();
        private:
            double deltaTime;
            double lastTime;
    };

    class Error : public std::runtime_error {
        public:
            using std::runtime_error::runtime_error;
    };

    enum class Key {
        Space = 32,
        Num0 = 48, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
        A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        Escape = 256, Enter, Tab, Backspace, Insert, Delete,
        Right, Left, Down, Up, PageUp, PageDown, Home, End,
        F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        LeftShift = 340, LeftControl, LeftAlt, LeftSuper, RightShift, RightControl, RightAlt
    };

    class Input {
        public:
            Input(GLFWwindow *window);

            static void Update();

            static bool IsKeyDown(Key key);
            static glm::vec2 GetMouseDelta();
            static void SetCursorLocked(bool locked);
        private:
            static inline GLFWwindow *s_window;
            static inline glm::vec2 s_lastMousePos;
            static inline glm::vec2 s_mouseDelta;
            static inline bool s_firstMouse;
    };

    class AudioEngine {
        public:
            AudioEngine();
            ~AudioEngine();

            void SetListenerPosition(const glm::vec3 &position);
            void SetListenerDirection(const glm::vec3 &forward, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));
        private:
            friend class AudioSource;

            struct Impl;
            std::unique_ptr<Impl> impl;
    };

    class Random {
        public:
            Random() = delete;

            static int   GetRandomInt(int min = 0, int max = 32767);
            static float GetRandomFloat(float min = 0.0f, float max = 32767.0f);
            static bool  GetRandomBool(double chanceForTrue = 0.5f);

            static void SetSeed(unsigned int seed);
        private:
            inline static std::mt19937 s_engine{ std::random_device{}() };
    };

    class Shader {
        public:
            Shader();
            ~Shader();
            
            void Render();

            unsigned int GetShaderProgram() const;
            int GetUniformLocation(const std::string &name);
        private:
            unsigned int shaderProgram;
            std::unordered_map<std::string, int> uniformCache;
    };

    class Component {
        public:
            Object *owner = nullptr;

            virtual ~Component() = default;
            virtual void Init() {}
            virtual void Update(double deltaTime) {}
            virtual void Draw() {}
            virtual void Cleanup() {}
    };

    class AudioSource : public Component {
        public:
            AudioSource(const std::string &path);
            ~AudioSource() override;

            void Play();
            void Stop();
            void SetLooping(bool loop);

            float volume = 1.0f;
            bool is3D = true;

            void Update(double deltaTime) override;
        private:
            struct Impl;
            std::unique_ptr<Impl> impl;
    };

    class Transform : public Component {
        public:
            glm::vec3 position = glm::vec3(0.0f);
            glm::vec3 rotation = glm::vec3(0.0f);
            glm::vec3 scale = glm::vec3(1.0f);

            glm::mat4 GetModelMatrix() const {
                glm::mat4 m = glm::translate(glm::mat4(1.0f), position);
                m = glm::rotate(m, rotation.x, glm::vec3(1, 0, 0));
                m = glm::rotate(m, rotation.y, glm::vec3(0, 1, 0));
                m = glm::rotate(m, rotation.z, glm::vec3(0, 0, 1));
                m = glm::scale(m, scale);
                return m;
            }
    };

    class Mesh : public Component {
        public:
            Mesh(shapes::MeshData mesh);

            void Draw() override;
            void Cleanup() override;
        private:
            std::vector<float> vertices = {};
            std::vector<unsigned int> indices = {};

            unsigned int VAO = 0;
            unsigned int VBO = 0;
            unsigned int EBO = 0;
    };

    class Texture {
        public:
            Texture(const char *path);
            ~Texture();

            unsigned int GetID() const;
        private:
            unsigned int id;
    };

    class Material : public Component {
        public:
            glm::vec4 color = glm::vec4(1.0f);
            std::shared_ptr<Texture> texture = nullptr;
            bool unlit = false;
    };

    enum class LightType {
        Directional,
        Point
    };

    class Light : public Component {
        public:
            LightType type = LightType::Directional;
            glm::vec3 color = glm::vec3(1.0f);
            float intensity = 1.0f;
            glm::vec3 direction = glm::vec3(-0.3f, -1.0f, -0.4f);
            float range = 10.0f;

            void Init() override;
            void Cleanup() override;
    };

    enum class ColliderType {
        Sphere,
        Box
    };

    class Collider : public Component {
        public:
            ColliderType type = ColliderType::Sphere;
            float radius = 0.5f;
            glm::vec3 halfExtents = glm::vec3(0.5f);
            bool isTrigger = false;
    };

    struct CollisionPair {
        Object *a;
        Object *b;
    };

    class Behavior : public Component {
        public:
            std::function<void(Object &self, double deltaTime)> onUpdate;

            void Update(double deltaTime) override {
                if (onUpdate) {
                    onUpdate(*owner, deltaTime);
                }
            }
    };

    class Object {
        public:
            Object() = default;
            ~Object();

            void Update(double deltaTime);
            void Draw();

            void AddTag(const std::string &tag);
            bool HasTag(const std::string &tag) const;
            void RemoveTag(const std::string &tag);

            template<typename T, typename... Args>
            T *AddComponent(Args&&... args) {
                auto comp = std::make_unique<T>(std::forward<Args>(args)...);
                T *ptr = comp.get();
                ptr->owner = this;

                auto key = std::type_index(typeid(T));
                auto existing = components.find(key);
                if (existing != components.end()) {
                    existing->second->Cleanup();
                }
                components[key] = std::move(comp);

                static_cast<Component *> (ptr)->Init();
                return ptr;
            }

            template<typename T>
            T *GetComponent() {
                auto it = components.find(std::type_index(typeid(T)));
                return it != components.end() ? static_cast<T *> (it->second.get()) : nullptr;
            }

            template<typename T>
            void RemoveComponent() {
                auto it = components.find(std::type_index(typeid(T)));
                if (it == components.end()) { return; }
                it->second->Cleanup();
                components.erase(it);
            }
        private:
            std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
            std::unordered_set<std::string> tags;
    };

    class Scene {
        public:
            void AddObject(const std::string &name, std::unique_ptr<Object> object);
            void RemoveObject(const std::string &name);
            Object *FindObject(const std::string &name);

            void Update(double deltaTime);
            void Draw();

            size_t Count() const;

            std::vector<Object *> FindObjectsWithTag(const std::string &tag);
            std::vector<CollisionPair> GetOverlaps();
        private:
            std::unordered_map<std::string, std::unique_ptr<Object>> objects;
    };

    enum class CameraMode {
        None,
        FPS,
        Space
    };

    class Camera {
        public:
            Camera(float aspectRatio = 1920.0f / 1080.0f);

            glm::vec3 position = glm::vec3(0.0f);
            glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f);

            float yaw   = -90.0f;
            float pitch = 0.0f;

            float fov       = 45.0f;
            float nearPlane = 0.1f;
            float farPlane  = 100.0f;

            float moveSpeed        = 3.0f;
            float mouseSensitivity = 0.1f;

            void SetMode(CameraMode newMode);
            void Update(double deltaTime);

            glm::vec3 GetForward() const;
            glm::vec3 GetRight() const;

            glm::mat4 GetViewMatrix() const;
            glm::mat4 GetProjectionMatrix() const;
        private:
            float m_aspectRatio;
            CameraMode m_mode = CameraMode::None;

            void UpdateFPS(double deltaTime);
            void UpdateSpace(double deltaTime);
    };

    enum class EaseType {
        Linear,
        EaseIn,
        EaseOut,
        EaseInOut
    };

    class Tween {
        public:
            static Tween *Position(Transform *target, glm::vec3 to, float duration, EaseType ease = EaseType::Linear);
            static Tween *Rotation(Transform *target, glm::vec3 to, float duration, EaseType ease = EaseType::Linear);
            static Tween *Scale(Transform *target, glm::vec3 to, float duration, EaseType ease = EaseType::Linear);

            static void UpdateAll(double deltaTime);

            void Cancel();
            bool IsFinished() const;
        private:
            enum class Field {
                Position,
                Rotation,
                Scale
            };

            Transform *target = nullptr;
            Field field;
            glm::vec3 from = glm::vec3(0.0f);
            glm::vec3 to   = glm::vec3(0.0f);
            float duration = 1.0f;
            float elapsed  = 0.0f;
            EaseType ease  = EaseType::Linear;
            bool finished  = false;
            bool cancelled = false;

            void Update(double deltaTime);
            static float ApplyEase(float t, EaseType ease);

            static inline std::vector<std::unique_ptr<Tween>> activeTweens;
    };
}

#endif