# Leap Engine

Leap Engine is a lightweight, 3D game engine written in C++. It is structured as a library designed to help developers create games and simulations efficiently.

## Features

- **Entity Component System (ECS):** Create custom components or use premade ones, then attach them to objects to instantly add functionality.
- **Scene Management:** Scenes act as central managers for your objects, allowing you to organize, update, and render your game worlds efficiently.
- **Rendering:** OpenGL 3.3 Core with built-in primitives (cube, sphere, cylinder, pyramid, plane, quad, polygon), OBJ model loading, textures and materials.
- **Lighting:** Directional and point lights (up to 8), plus unlit materials for self-illuminated objects.
- **Audio:** Positional and non-positional sound playback (powered by miniaudio).
- **Collision Detection:** Sphere and axis-aligned box colliders with overlap queries.
- **Tween System:** A built-in system dedicated to animating your objects smoothly using various interpolation modes.
- **Utilities:** Object tags, prefab helpers, a `Behavior` component for quick custom logic, input handling, and a free-fly/FPS camera.

## Limitations

- One `Window` and one active scene per process.
- At most 8 lights are used per frame.
- Collision is detection only (no physics), and boxes are axis-aligned.
- Tweens keep a pointer to their target `Transform`; cancel them before destroying the object.
- Don't add or remove components or objects from inside an update callback such as `Behavior::onUpdate`.
- Raw OpenGL calls aren't available to your code.

## Installation

Leap Engine is built from source as part of your own CMake project, so there are no prebuilt binaries to match against your compiler.

### Requirements

- CMake 3.15 or newer
- A C++17 compiler
- A GPU and driver that support OpenGL 3.3 Core
- Internet access the first time you configure (CMake downloads GLFW 3.4)

| Platform | Compiler | Status |
|----------|----------|--------|
| Windows 10/11 | MinGW-w64 GCC 14.2.0 | Tested |
| Linux | GCC | Untested |
| macOS | Apple Clang | Untested |
| Windows | MSVC | Untested |

**Linux only:** GLFW needs the X11 development packages.

```bash
# Debian / Ubuntu
sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

### Add Leap Engine to your project

**Option A: FetchContent (recommended)**

```cmake
include(FetchContent)
FetchContent_Declare(LeapEngine
    GIT_REPOSITORY https://github.com/StrivingTable67-User/Leap-Engine.git
    GIT_TAG v1.0.0)
FetchContent_MakeAvailable(LeapEngine)
```

**Option B: git submodule or vendored copy**

```bash
git submodule add https://github.com/StrivingTable67-User/Leap-Engine.git libs/LeapEngine
```

```cmake
add_subdirectory(libs/LeapEngine)
```

### Minimal CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyGame LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(LeapEngine
    GIT_REPOSITORY https://github.com/StrivingTable67-User/Leap-Engine.git
    GIT_TAG v1.0.0)
FetchContent_MakeAvailable(LeapEngine)

add_executable(MyGame main.cpp)
target_link_libraries(MyGame PRIVATE LeapEngine::LeapEngine)
```

That one `target_link_libraries` line gives your project the include path, C++17, and everything Leap Engine depends on (GLFW, OpenGL, and the platform libraries).

### Build

```bash
cmake -S . -B build
cmake --build build
```

On Windows with MinGW, add `-G "MinGW Makefiles"` to the first command. The first build also compiles GLFW, so it takes longer than later ones.

Run your program from the folder your assets are in. Relative paths, such as texture paths, are resolved from the working directory.

### Check that it works

Put the program from [Usage](#usage) in `main.cpp` and run it. You should see a lit orange sphere. Press Escape to quit.

### Notes

- Leap Engine includes glad. Don't add your own `glad.c` to the same project.
- Create the `Window` first and let it be destroyed last. Meshes, shaders and textures need its OpenGL context to still exist when they are destroyed.
- One `Window` and one active scene per process are supported.

### Troubleshooting

- **`Could NOT find X11` or missing X11 headers (Linux):** install the packages listed above.
- **`Failed to Initialize GLFW`:** no display is available, for example over SSH or on a headless machine.
- **`Failed to Create GLFW Window`:** your graphics driver doesn't provide OpenGL 3.3 Core. Update your drivers.
- **Configuring fails while downloading GLFW (offline or behind a proxy):** download the GLFW 3.4 source yourself and configure with `-DFETCHCONTENT_SOURCE_DIR_GLFW=/path/to/glfw`.
- **Duplicate or undefined symbol errors mentioning glad, such as `GLVersion`:** your project is compiling its own `glad.c`. Remove it.

## Usage

Here is a minimal, working program that demonstrates how to create an object, attach components, set up a lighting scene, and run the main game loop.

```C++
#include <leap.h>
#include <memory> // For smart pointers
#include <iostream> // For std::cerr

int main() {
    try {
        // Initialize Variables
        leap::Window window; // Optional args: (unsigned int width, unsigned int height, const char *title, bool    isFullscreen)
        leap::Time time;
        leap::Input input(window.GetNativeWindow());
        leap::Shader shader;
        leap::Camera camera(window.GetAspectRatio());

        // Set Default Variables
        leap::SetDefaultInput(&input);
        leap::SetDefaultShader(&shader);
        leap::SetDefaultCamera(&camera);

        // Change Camera Settings
        camera.SetMode(leap::CameraMode::Space); // Use Space mode (There is Space, FPS and None (Default))
        camera.position = leap::vec3(0.0f, 0.0f, 8.0f); // Move the camera back to see the sphere

        // Create a Sphere Mesh
        const leap::shapes::MeshData sphereMesh = leap::shapes::LoadSphere();
        /* All Mesh Types:
         * LoadCube()
         * LoadPlane()
         * LoadSphere(unsigned int stacks = 16, unsigned int slices = 16)
         * LoadPyramid(unsigned int sides = 4)
         * LoadCylinder(unsigned int stacks = 16, unsigned int slices = 16)
         * LoadQuad()
         * LoadPolygon(unsigned int sides = 90)
         * LoadOBJ(const std::string &path) (Loads an external object)
         */

        // Create the Object
        std::unique_ptr<leap::Object> obj = std::make_unique<leap::Object>(); // Creating the Object Variable
        leap::Transform *objTransform = obj->AddComponent<leap::Transform>(); // Adding the Transform
        objTransform->scale = leap::vec3(2.5f); // Initialize the Scale (2.5, 2.5, 2.5) of the Transform (There is also position and Rotation (radians))
        obj->AddComponent<leap::Mesh>(sphereMesh); // Adds the mesh from earlier to the object (No variable because we don't change any of it's properties)
        leap::Material *objMat = obj->AddComponent<leap::Material>(); // Creates a material for the object (for colors and textures)
        objMat->color = leap::vec4(1.0f, 0.5f, 0.0f, 1.0f); // Assigns the color (RGBA) with normalized values (0-1 instead of 0-255)
        /* To Assign a Texture to the Object:
         * 1. Before creating the object, add: std::shared_ptr<leap::Texture> texture = std::make_shared<leap::Texture> ("somePathThatLeadsToAnImage");
         * 2. After creating objMat, add: objMat->texture = texture;
         * To Make an unlit Material (unaffected by lights and is always bright), add: objMat->unlit = true;
         */

        // Create a light to illuminate the object
        std::unique_ptr<leap::Object> light = std::make_unique<leap::Object>();
        leap::Transform *lightTransform = light->AddComponent<leap::Transform>();
        lightTransform->position = leap::vec3(5.0f); // Sets the position to (5.0, 5.0, 5.0)
        leap::Light *lightLight = light->AddComponent<leap::Light>(); // Add a light component to the light
        lightLight->range = 20.0f; // The range of the light
        lightLight->type = leap::LightType::Point; // Makes a point light, there is LightType::Point and    LightType::Directional (Default)

        // Create a scene for the objects
        std::unique_ptr<leap::Scene> scene = std::make_unique<leap::Scene>(); // Creating the scene variable
        scene->AddObject("object", std::move(obj)); // Add obj to the scene
        scene->AddObject("light", std::move(light)); // Add light to the scene

        // Main Frame Loop
        while (!window.ShouldClose()) {
            input.Update(); // Update the input handling
            double dt = time.GetDeltaTime(); // Create a variable for deltaTime instead of calling GetDeltaTime a bunch
            camera.Update(dt); // Update the camera

            // Update logic goes here
            scene->Update(dt);

            // Close the Window if Escape is Pressed
            if (input.IsKeyDown(leap::Key::Escape)) {
                window.Close();
            }

            window.ClearBackground(0.0f, 0.0f, 0.0f); // Clear the Background (r, g, b) with normalized colors
            shader.Render(); // Render the shader to draw stuff

            // Draw logic goes here
            scene->Draw();

            window.Render(); // Render the window
        }

        // Remove all objects from the scene
        scene->RemoveObject("object");
        scene->RemoveObject("light");

        // Everything else gets deallocated automatically due to smart pointers and destructors
    } catch (const leap::Error &e) {
        // Error handling
        std::cerr << "ERROR: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

### Third-Party Software

Leap Engine's repository includes the following, under their own licenses. Full texts are in the [`licenses/`](licenses/) folder.

| Library | Used for | License |
|---------|----------|---------|
| [GLM](https://glm.g-truc.net) | Math | Happy Bunny License or MIT License (Leap Engine uses it under MIT) |
| [miniaudio](https://github.com/mackron/miniaudio) | Audio | Public domain or MIT-0 |
| [stb_image](https://github.com/nothings/stb) | Image loading | MIT or public domain |
| [glad](https://github.com/Dav1dde/glad) | OpenGL loader | See `licenses/glad.txt` |

[GLFW](https://www.glfw.org) (zlib/libpng license) is downloaded at build time and is not part of this repository.