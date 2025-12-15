//
// Material Demo - Showcases Unity-like material system
//

#include "../GraphicsEngine.h"
#include "../Assets/Scripts/rotator.h"
#include "../Assets/Scripts/cameraController.h"
#include <cmath>

int main()
{
    // Create scene
    Scene scene;
    scene.name = "MaterialDemo";
    scene.backgroundColor = color(0.1f, 0.1f, 0.15f);

    std::cout << "Creating camera..." << std::endl;
    
    // Create camera
    auto camera = scene.createGameObject("MainCamera");
    camera->transform.setPosition(vec3(0, 5, 15));
    camera->transform.setRotation(vec3(0, M_PI, 0));
    camera->addComponent<CameraComponent>();
    camera->addComponent<CameraController>();

    std::cout << "Creating ground..." << std::endl;
    
    // Create ground (material will be set after OpenGL init)
    auto ground = scene.createGameObject("Ground");
    ground->addComponent<MeshFilter>()->setMesh(Mesh::createPlane(20, 20));
    auto groundRenderer = ground->addComponent<MeshRenderer>();

    std::cout << "Creating test objects..." << std::endl;

    // Row 1: PBR materials with varying metallic (0.0 to 1.0)
    std::vector<GameObject*> pbrRow;
    for (int x = 0; x < 4; x++)
    {
        auto obj = scene.createGameObject("PBR_Metallic_" + std::to_string(x));
        obj->transform.setPosition(vec3(x * 3.0f - 4.5f, 1.5f, -6.0f));
        obj->addComponent<MeshFilter>()->setMesh(Mesh::createSphere(1.0f, 2));
        obj->addComponent<MeshRenderer>();
        obj->addComponent<Rotator>();
        pbrRow.push_back(obj);
    }

    // Row 2: PBR materials with varying smoothness (0.0 to 1.0)
    std::vector<GameObject*> smoothRow;
    for (int x = 0; x < 4; x++)
    {
        auto obj = scene.createGameObject("PBR_Smooth_" + std::to_string(x));
        obj->transform.setPosition(vec3(x * 3.0f - 4.5f, 1.5f, -2.0f));
        obj->addComponent<MeshFilter>()->setMesh(Mesh::createSphere(1.0f, 2));
        obj->addComponent<MeshRenderer>();
        obj->addComponent<Rotator>();
        smoothRow.push_back(obj);
    }

    // Row 3: Unlit materials with different colors
    std::vector<GameObject*> unlitRow;
    for (int x = 0; x < 4; x++)
    {
        auto obj = scene.createGameObject("Unlit_" + std::to_string(x));
        obj->transform.setPosition(vec3(x * 3.0f - 4.5f, 1.5f, 2.0f));
        obj->addComponent<MeshFilter>()->setMesh(Mesh::createCube());
        obj->addComponent<MeshRenderer>();
        obj->addComponent<Rotator>();
        unlitRow.push_back(obj);
    }

    // Row 4: Standard Specular materials
    std::vector<GameObject*> specularRow;
    for (int x = 0; x < 4; x++)
    {
        auto obj = scene.createGameObject("Specular_" + std::to_string(x));
        obj->transform.setPosition(vec3(x * 3.0f - 4.5f, 1.5f, 6.0f));
        obj->addComponent<MeshFilter>()->setMesh(Mesh::createSphere(1.0f, 2));
        obj->addComponent<MeshRenderer>();
        obj->addComponent<Rotator>();
        specularRow.push_back(obj);
    }

    // Center cube - mixed materials demo
    auto centerCube = scene.createGameObject("CenterCube");
    centerCube->transform.setPosition(vec3(0, 1.5f, 0));
    centerCube->addComponent<MeshFilter>()->setMesh(Mesh::createCube());
    auto centerRenderer = centerCube->addComponent<MeshRenderer>();
    centerCube->addComponent<Rotator>();

    std::cout << "\n=== CPP Graphics Engine - Material System Demo ===" << std::endl;
    std::cout << "Scene: " << scene.name << std::endl;
    std::cout << "Objects: " << scene.getAllGameObjects().size() << std::endl;
    std::cout << "\nMaterial Types:" << std::endl;
    std::cout << "  - Standard (PBR): Row 1 (Red spheres, varying metallic)" << std::endl;
    std::cout << "  - Standard (PBR): Row 2 (Green spheres, varying smoothness)" << std::endl;
    std::cout << "  - Unlit: Row 3 (Colored cubes, no lighting)" << std::endl;
    std::cout << "  - Standard Specular: Row 4 (Blue spheres, varying specular)" << std::endl;
    std::cout << "  - Center: PBR cube with high smoothness" << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  WASD - Move" << std::endl;
    std::cout << "  Space/Shift - Up/Down" << std::endl;
    std::cout << "  Mouse - Look" << std::endl;
    std::cout << "  Left Ctrl - Sprint" << std::endl;
    std::cout << "  ESC - Exit\n" << std::endl;

    // Set up materials after OpenGL initialization
    scene.onOpenGLReady([&](Scene& s) {
            std::cout << "OpenGL context ready - Creating materials..." << std::endl;
            
            // Ground material (dark gray, slightly rough)
            auto groundMat = BuiltinMaterials::createStandard();
            groundMat->setColor("_Color", color(0.3f, 0.3f, 0.3f));
            groundMat->setFloat("_Metallic", 0.0f);
            groundMat->setFloat("_Smoothness", 0.4f);
            groundRenderer->setMaterial(groundMat);
            
            // Row 1: Varying metallic (red spheres)
            for (int x = 0; x < 4; x++) {
                auto mat = BuiltinMaterials::createStandard();
                mat->setColor("_Color", color(0.9f, 0.1f, 0.1f));
                mat->setFloat("_Metallic", x / 3.0f);  // 0.0, 0.33, 0.66, 1.0
                mat->setFloat("_Smoothness", 0.8f);
                pbrRow[x]->getComponent<MeshRenderer>()->setMaterial(mat);
            }
            
            // Row 2: Varying smoothness (green spheres)
            for (int x = 0; x < 4; x++) {
                auto mat = BuiltinMaterials::createStandard();
                mat->setColor("_Color", color(0.1f, 0.9f, 0.1f));
                mat->setFloat("_Metallic", 0.2f);
                mat->setFloat("_Smoothness", x / 3.0f);  // 0.0, 0.33, 0.66, 1.0
                smoothRow[x]->getComponent<MeshRenderer>()->setMaterial(mat);
            }
            
            // Row 3: Unlit with rainbow colors
            for (int x = 0; x < 4; x++) {
                auto mat = BuiltinMaterials::createUnlit();
                float hue = x / 4.0f;
                color cubeColor(
                    std::abs(std::sin(hue * M_PI * 2)),
                    std::abs(std::sin((hue + 0.33f) * M_PI * 2)),
                    std::abs(std::sin((hue + 0.66f) * M_PI * 2))
                );
                mat->setColor("_Color", cubeColor);
                unlitRow[x]->getComponent<MeshRenderer>()->setMaterial(mat);
            }
            
            // Row 4: Standard Specular with varying specular color
            for (int x = 0; x < 4; x++) {
                auto mat = BuiltinMaterials::createStandardSpecular();
                mat->setColor("_Color", color(0.2f, 0.2f, 0.8f));
                float specAmount = x / 3.0f;
                mat->setColor("_SpecColor", color(specAmount, specAmount, specAmount));
                mat->setFloat("_Smoothness", 0.8f);
                specularRow[x]->getComponent<MeshRenderer>()->setMaterial(mat);
            }
            
            // Center cube: Shiny metallic PBR
            auto centerMat = BuiltinMaterials::createStandard();
            centerMat->setColor("_Color", color(0.9f, 0.7f, 0.2f));  // Gold-ish
            centerMat->setFloat("_Metallic", 0.9f);
            centerMat->setFloat("_Smoothness", 0.95f);
            centerRenderer->setMaterial(centerMat);
            
            std::cout << "Materials created successfully!" << std::endl;
        });

    // Run engine
    Engine::runOpenGL(scene, 1280, 720, "CPP Graphics Engine - Material System", 60);

    return 0;
}
