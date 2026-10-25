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
