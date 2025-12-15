//
// Main Entry Point
// Keep this simple - just setup scene and run engine
//

#include "GraphicsEngine.h"
#include "Assets/Scripts/rotator.h"
#include "Assets/Scripts/cameraController.h"
#include <cmath>

int main()
{
    // Create scene
    Scene scene;
    scene.name = "MainScene";
    scene.backgroundColor = color(0.1f, 0.1f, 0.15f);

    // Create camera
    auto camera = scene.createGameObject("MainCamera");
    camera->transform.setPosition(vec3(0, 5, 15));
    camera->transform.setRotation(vec3(0, M_PI, 0));  // Rotate 180° to look at -Z
    camera->addComponent<CameraComponent>();
    camera->addComponent<CameraController>();

    // Create ground
    auto ground = scene.createGameObject("Ground");
    ground->setMesh(Mesh::createPlane(20, 20));
    for (auto& v : ground->getMesh()->vertices) {
        v.vertexColor = color(0.3f, 0.5f, 0.3f);
    }

    // Create some objects
    for (int x = -3; x <= 3; x++)
    {
        for (int z = -3; z <= 3; z++)
        {
            float hue = (float)(x + 3) / 6.0f;
            color objColor(
                std::abs(std::sin(hue * M_PI)),
                std::abs(std::cos(hue * M_PI)),
                std::abs(std::sin((hue + 0.5f) * M_PI))
            );

            auto obj = scene.createGameObject("Cube_" + std::to_string(x) + "_" + std::to_string(z));
            obj->transform.setPosition(vec3(x * 3.0f, 1.5f, z * 3.0f));

            if ((x + z) % 2 == 0) {
                obj->setMesh(Mesh::createCube());
                // Apply tint to cube's existing per-face colors for variety
                float tintStrength = 0.6f;  // How much of the tint to apply
                for (auto& v : obj->getMesh()->vertices) {
                    // Blend original face color with tint color
                    v.vertexColor = color(
                        v.vertexColor.x() * (1.0f - tintStrength) + objColor.x() * tintStrength,
                        v.vertexColor.y() * (1.0f - tintStrength) + objColor.y() * tintStrength,
                        v.vertexColor.z() * (1.0f - tintStrength) + objColor.z() * tintStrength
                    );
                }
            } else {
                obj->setMesh(Mesh::createSphere(1.0f, 2));
                // Spheres get solid color
                for (auto& v : obj->getMesh()->vertices) {
                    v.vertexColor = objColor;
                }
            }

            // Add rotator to some objects
            if (x % 2 == 0) {
                obj->addComponent<Rotator>();
            }
        }
    }

    // Optional: Save scene
    // SceneSerializer::saveSceneToAssets(scene, "MainScene");

    // Optional: Load scene
    // Scene loadedScene = SceneSerializer::loadSceneFromAssets("MainScene");

    std::cout << "=== CPP Graphics Engine ===" << std::endl;
    std::cout << "Scene: " << scene.name << std::endl;
    std::cout << "Objects: " << scene.getAllGameObjects().size() << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  WASD - Move" << std::endl;
    std::cout << "  Space/Shift - Up/Down" << std::endl;
    std::cout << "  Right Mouse - Look" << std::endl;
    std::cout << "  Left Ctrl - Sprint" << std::endl;
    std::cout << "  ESC - Exit\n" << std::endl;

    // Run engine
    Engine::runOpenGL(scene, 1280, 720, "CPP Graphics Engine");

    return 0;
}
