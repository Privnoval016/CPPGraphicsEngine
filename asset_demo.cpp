//
// Asset Loading Demo - Shows loading models and materials from files
//

#include "GraphicsEngine.h"
#include "Assets/Scripts/rotator.h"
#include "Assets/Scripts/cameraController.h"

int main()
{
    // Create scene
    Scene scene;
    scene.name = "AssetLoadingDemo";
    scene.backgroundColor = color(0.1f, 0.1f, 0.15f);

    std::cout << "\n=== CPP Graphics Engine - Asset Loading Demo ===" << std::endl;
    
    // Create camera
    auto camera = scene.createGameObject("MainCamera");
    camera->transform.setPosition(vec3(0, 5, 15));
    camera->transform.setRotation(vec3(0, M_PI, 0));
    camera->addComponent<CameraComponent>();
    camera->addComponent<CameraController>();

    // Create ground
    auto ground = scene.createGameObject("Ground");
    ground->addComponent<MeshFilter>()->setMesh(Mesh::createPlane(20, 20));
    auto groundRenderer = ground->addComponent<MeshRenderer>();

    // Load pyramid model
    std::cout << "Loading 3D model..." << std::endl;
    auto pyramidMesh = ModelLoader::loadFromFile("Assets/Models/AmongUs.obj");
    
    if (pyramidMesh) {
        // Create multiple pyramids with different materials
        std::vector<std::pair<std::string, vec3>> pyramids = {
            {"Red Metal Pyramid", vec3(-6, 1, 0)},
            {"Gold Pyramid", vec3(-2, 1, 0)},
            {"Green Plastic Pyramid", vec3(2, 1, 0)},
            {"Blue Emissive Pyramid", vec3(6, 1, 0)}
        };
        
        for (const auto& [name, pos] : pyramids) {
            auto obj = scene.createGameObject(name);
            obj->transform.setPosition(pos);
            obj->addComponent<MeshFilter>()->setMesh(pyramidMesh);
            obj->addComponent<MeshRenderer>();
            obj->addComponent<Rotator>()->rotationSpeed = vec3(0, 1, 0);
        }
    } else {
        std::cerr << "Failed to load pyramid model - using default cubes" << std::endl;
        
        // Fallback to cubes
        for (int i = 0; i < 4; i++) {
            auto obj = scene.createGameObject("Cube_" + std::to_string(i));
            obj->transform.setPosition(vec3(i * 4.0f - 6.0f, 1, 0));
            obj->addComponent<MeshFilter>()->setMesh(Mesh::createCube());
            obj->addComponent<MeshRenderer>();
            obj->addComponent<Rotator>()->rotationSpeed = vec3(0, 1, 0);
        }
    }

    // Create some built-in primitive objects
    auto sphere = scene.createGameObject("Stone Sphere");
    sphere->transform.setPosition(vec3(0, 1.5f, -5));
    sphere->addComponent<MeshFilter>()->setMesh(Mesh::createSphere(1.0f, 2));
    auto sphereRenderer = sphere->addComponent<MeshRenderer>();

    auto cube = scene.createGameObject("Red Metal Cube");
    cube->transform.setPosition(vec3(0, 1, 5));
    cube->addComponent<MeshFilter>()->setMesh(Mesh::createCube());
    auto cubeRenderer = cube->addComponent<MeshRenderer>();
    cube->addComponent<Rotator>()->rotationSpeed = vec3(0.5f, 1, 0.3f);

    std::cout << "Scene: " << scene.name << std::endl;
    std::cout << "Objects: " << scene.getAllGameObjects().size() << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  WASD - Move" << std::endl;
    std::cout << "  Space/Shift - Up/Down" << std::endl;
    std::cout << "  Mouse - Look" << std::endl;
    std::cout << "  Left Ctrl - Sprint" << std::endl;
    std::cout << "  ESC - Exit\n" << std::endl;

    // Run engine with OpenGL-ready callback
    Engine::runOpenGL(scene, 1280, 720, "CPP Graphics Engine - Asset Loading", 60, 
        [&](Scene& s) {
            std::cout << "OpenGL context ready - Loading materials from files..." << std::endl;
            
            // Load materials from files
            auto redMetal = MaterialSerializer::loadFromFile("Assets/Materials/red_metal.mat");
            auto gold = MaterialSerializer::loadFromFile("Assets/Materials/gold.mat");
            auto greenPlastic = MaterialSerializer::loadFromFile("Assets/Materials/green_plastic.mat");
            auto blueEmissive = MaterialSerializer::loadFromFile("Assets/Materials/blue_emissive.mat");
            auto stone = MaterialSerializer::loadFromFile("Assets/Materials/stone.mat");
            
            // Apply materials to objects
            if (pyramidMesh) {
                auto redPyramid = s.findGameObject("Red Metal Pyramid");
                if (redPyramid && redMetal) {
                    redPyramid->getComponent<MeshRenderer>()->setMaterial(redMetal);
                }
                
                auto goldPyramid = s.findGameObject("Gold Pyramid");
                if (goldPyramid && gold) {
                    goldPyramid->getComponent<MeshRenderer>()->setMaterial(gold);
                }
                
                auto greenPyramid = s.findGameObject("Green Plastic Pyramid");
                if (greenPyramid && greenPlastic) {
                    greenPyramid->getComponent<MeshRenderer>()->setMaterial(greenPlastic);
                }
                
                auto bluePyramid = s.findGameObject("Blue Emissive Pyramid");
                if (bluePyramid && blueEmissive) {
                    bluePyramid->getComponent<MeshRenderer>()->setMaterial(blueEmissive);
                }
            }
            
            // Apply materials to primitives
            if (stone) {
                sphereRenderer->setMaterial(stone);
            }
            
            if (redMetal) {
                cubeRenderer->setMaterial(redMetal);
            }
            
            // Ground material
            auto groundMat = BuiltinMaterials::createStandard();
            groundMat->setColor("_Color", color(0.3f, 0.3f, 0.3f));
            groundMat->setFloat("_Metallic", 0.0f);
            groundMat->setFloat("_Smoothness", 0.4f);
            groundRenderer->setMaterial(groundMat);
            
            std::cout << "Materials loaded successfully!" << std::endl;
            std::cout << "\nObjects in scene:" << std::endl;
            std::cout << "  - 4 Pyramids (loaded from OBJ file)" << std::endl;
            std::cout << "  - 1 Sphere with stone material" << std::endl;
            std::cout << "  - 1 Cube with red metal material" << std::endl;
            std::cout << "  - Ground plane\n" << std::endl;
        });

    return 0;
}
