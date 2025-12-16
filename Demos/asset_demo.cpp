//
// Asset Loading Demo - Shows loading models and materials from files
//

#include "../GraphicsEngine.h"
#include "../Assets/Scripts/rotator.h"
#include "../Assets/Scripts/cameraController.h"

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
    auto pyramidMesh = ModelLoader::loadFromFile("Assets/Models/pyramid.obj");
    
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

    // Set up materials after OpenGL initialization
    scene.onOpenGLReady([&](Scene& s) {
            std::cout << "OpenGL context ready - Loading materials from files..." << std::endl;
            
            // Load materials from files (may return nullptr if files don't exist)
            auto redMetal = MaterialSerializer::loadFromFile("Assets/Materials/red_metal.mat");
            auto gold = MaterialSerializer::loadFromFile("Assets/Materials/gold.mat");
            auto greenPlastic = MaterialSerializer::loadFromFile("Assets/Materials/green_plastic.mat");
            auto blueEmissive = MaterialSerializer::loadFromFile("Assets/Materials/blue_emissive.mat");
            auto stone = MaterialSerializer::loadFromFile("Assets/Materials/stone.mat");
            
            // Create fallback materials if files don't exist
            if (!redMetal) {
                redMetal = BuiltinMaterials::createStandard();
                redMetal->setColor("_Color", color(0.8f, 0.1f, 0.1f));
                redMetal->setFloat("_Metallic", 0.8f);
                redMetal->setFloat("_Smoothness", 0.7f);
            }
            if (!gold) {
                gold = BuiltinMaterials::createStandard();
                gold->setColor("_Color", color(1.0f, 0.8f, 0.2f));
                gold->setFloat("_Metallic", 1.0f);
                gold->setFloat("_Smoothness", 0.9f);
            }
            if (!greenPlastic) {
                greenPlastic = BuiltinMaterials::createStandard();
                greenPlastic->setColor("_Color", color(0.1f, 0.8f, 0.2f));
                greenPlastic->setFloat("_Metallic", 0.0f);
                greenPlastic->setFloat("_Smoothness", 0.6f);
            }
            if (!blueEmissive) {
                blueEmissive = BuiltinMaterials::createStandard();
                blueEmissive->setColor("_Color", color(0.2f, 0.4f, 1.0f));
                blueEmissive->setFloat("_Metallic", 0.0f);
                blueEmissive->setFloat("_Smoothness", 0.9f);
            }
            if (!stone) {
                stone = BuiltinMaterials::createStandard();
                stone->setColor("_Color", color(0.5f, 0.5f, 0.5f));
                stone->setFloat("_Metallic", 0.1f);
                stone->setFloat("_Smoothness", 0.3f);
            }
            
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
            
            // Ground material - PBR with textures
            auto groundMat = BuiltinMaterials::createStandard();
            
            // Load ground textures with path tracking
            std::string diffusePath = "Assets/Textures/Ground/GroundDiffuse.jpg";
            std::string specularPath = "Assets/Textures/Ground/GroundSpecular.png";
            std::string displacementPath = "Assets/Textures/Ground/GroundDisplacement.png";
            
            auto groundDiffuse = TextureLoader::loadFromFile(diffusePath);
            auto groundSpecular = TextureLoader::loadFromFile(specularPath);
            auto groundDisplacement = TextureLoader::loadFromFile(displacementPath);
            
            if (groundDiffuse) {
                groundMat->setTexture("_MainTex", groundDiffuse, diffusePath);
                // Flag is set automatically by setTexture()
            }
            if (groundSpecular) {
                // Use specular map as metallic/smoothness map
                groundMat->setTexture("_MetallicGlossMap", groundSpecular, specularPath);
                // Flag is set automatically by setTexture()
            }
            if (groundDisplacement) {
                // Use displacement as bump/normal map
                groundMat->setTexture("_BumpMap", groundDisplacement, displacementPath);
                groundMat->setFloat("_BumpScale", 0.3f);
                // Flag is set automatically by setTexture()
            }
            
            groundMat->setName("Ground");
            groundMat->setColor("_Color", color(1.0f, 1.0f, 1.0f)); // White tint to show texture colors
            groundMat->setFloat("_Metallic", 0.1f);
            groundMat->setFloat("_Smoothness", 0.3f);
            groundRenderer->setMaterial(groundMat);
            
            // Save ground material to file for reuse
            MaterialSerializer::saveToFile(*groundMat, "Assets/Materials/ground.mat");
            std::cout << "Ground material saved to Assets/Materials/ground.mat" << std::endl;
            
            std::cout << "Materials loaded successfully!" << std::endl;
            std::cout << "\nObjects in scene:" << std::endl;
            std::cout << "  - 4 Pyramids (loaded from OBJ file)" << std::endl;
            std::cout << "  - 1 Sphere with stone material" << std::endl;
            std::cout << "  - 1 Cube with red metal material" << std::endl;
            std::cout << "  - Ground plane\n" << std::endl;
        });

    // Run engine
    Engine::runOpenGL(scene, 1280, 720, "CPP Graphics Engine - Asset Loading", 60);

    return 0;
}
