//
// Custom Material Demo - Shows how to register and serialize custom material types
//

#include "../GraphicsEngine.h"
#include "../Assets/Scripts/rotator.h"
#include "../Assets/Scripts/cameraController.h"

// Example: Custom Toon Shader Material
std::shared_ptr<Material> createToonMaterial()
{
    // In a real implementation, you'd create a custom shader here
    // For demo purposes, we'll use the Standard shader but customize it
    auto material = BuiltinMaterials::createStandard();
    material->setName("Toon");
    
    // Set default toon shader properties
    material->setColor("_Color", color(1.0f, 0.5f, 0.2f));
    material->setFloat("_Metallic", 0.0f);
    material->setFloat("_Smoothness", 0.1f);  // Low smoothness for toon look
    material->setInt("_ToonSteps", 3);  // Number of shading bands
    
    return material;
}

// Example: Custom Hologram Material
std::shared_ptr<Material> createHologramMaterial()
{
    auto material = BuiltinMaterials::createUnlit();
    material->setName("Hologram");
    
    material->setColor("_Color", color(0.0f, 1.0f, 1.0f));  // Cyan
    material->setFloat("_Opacity", 0.5f);
    material->setFloat("_ScanlineSpeed", 2.0f);
    material->setFloat("_FlickerIntensity", 0.1f);
    
    return material;
}

int main()
{
    // Register custom material types before loading any files
    std::cout << "Registering custom material types..." << std::endl;
    MaterialSerializer::registerMaterialType("Toon", createToonMaterial);
    MaterialSerializer::registerMaterialType("Hologram", createHologramMaterial);
    
    std::cout << "✓ Custom material types registered" << std::endl;
    
    // Create scene
    Scene scene;
    scene.name = "CustomMaterialDemo";
    scene.backgroundColor = color(0.05f, 0.05f, 0.1f);

    // Camera
    auto camera = scene.createGameObject("MainCamera");
    camera->transform.setPosition(vec3(0, 2, 8));
    camera->transform.setRotation(vec3(0, M_PI, 0));
    camera->addComponent<CameraComponent>();
    camera->addComponent<CameraController>();

    // Ground
    auto ground = scene.createGameObject("Ground");
    ground->addComponent<MeshFilter>()->setMesh(Mesh::createPlane(20, 20));
    auto groundRenderer = ground->addComponent<MeshRenderer>();

    // Objects with custom materials
    auto toonCube = scene.createGameObject("ToonCube");
    toonCube->transform.setPosition(vec3(-3, 1, 0));
    toonCube->addComponent<MeshFilter>()->setMesh(Mesh::createCube());
    auto toonRenderer = toonCube->addComponent<MeshRenderer>();
    toonCube->addComponent<Rotator>()->rotationSpeed = vec3(0.2f, 0.5f, 0.1f);

    auto holoSphere = scene.createGameObject("HoloSphere");
    holoSphere->transform.setPosition(vec3(3, 1, 0));
    holoSphere->addComponent<MeshFilter>()->setMesh(Mesh::createSphere(1.0f));
    auto holoRenderer = holoSphere->addComponent<MeshRenderer>();
    holoSphere->addComponent<Rotator>()->rotationSpeed = vec3(0.1f, 0.3f, 0.2f);

    // Standard material comparison
    auto standardSphere = scene.createGameObject("StandardSphere");
    standardSphere->transform.setPosition(vec3(0, 1, -3));
    standardSphere->addComponent<MeshFilter>()->setMesh(Mesh::createSphere(1.0f));
    auto standardRenderer = standardSphere->addComponent<MeshRenderer>();

    std::cout << "\nDemo Info:" << std::endl;
    std::cout << "  Left (Orange Cube)   - Custom Toon material" << std::endl;
    std::cout << "  Right (Cyan Sphere)  - Custom Hologram material" << std::endl;
    std::cout << "  Back (Sphere)        - Standard PBR material" << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  WASD - Move" << std::endl;
    std::cout << "  Mouse - Look" << std::endl;
    std::cout << "  Space/Shift - Up/Down" << std::endl;
    std::cout << "  ESC - Exit\n" << std::endl;

    // Set up materials after OpenGL initialization
    scene.onOpenGLReady([&](Scene& s) {
            // Create and save example custom material files (after OpenGL init)
            std::cout << "\nCreating example custom material files..." << std::endl;
            
            auto toonMat = createToonMaterial();
            toonMat->setColor("_Color", color(1.0f, 0.3f, 0.0f));  // Orange
            toonMat->setInt("_ToonSteps", 4);
            MaterialSerializer::saveToFile(*toonMat, "Assets/Materials/custom_toon.mat");
            std::cout << "  Saved: Assets/Materials/custom_toon.mat" << std::endl;
            
            auto holoMat = createHologramMaterial();
            holoMat->setColor("_Color", color(0.0f, 1.0f, 0.5f));  // Green-cyan
            holoMat->setFloat("_Opacity", 0.7f);
            holoMat->setFloat("_ScanlineSpeed", 3.0f);
            MaterialSerializer::saveToFile(*holoMat, "Assets/Materials/custom_hologram.mat");
            std::cout << "  Saved: Assets/Materials/custom_hologram.mat" << std::endl;
            
            // Load them back to verify serialization works
            std::cout << "\nLoading custom materials from files..." << std::endl;
            auto loadedToon = MaterialSerializer::loadFromFile("Assets/Materials/custom_toon.mat");
            auto loadedHolo = MaterialSerializer::loadFromFile("Assets/Materials/custom_hologram.mat");
            
            if (loadedToon) {
                std::cout << "  ✓ Loaded Toon material" << std::endl;
                toonRenderer->setMaterial(loadedToon);
            }
            if (loadedHolo) {
                std::cout << "  ✓ Loaded Hologram material" << std::endl;
                holoRenderer->setMaterial(loadedHolo);
            }
            
            // Ground
            auto groundMat = BuiltinMaterials::createStandard();
            groundMat->setColor("_Color", color(0.2f, 0.2f, 0.25f));
            groundMat->setFloat("_Metallic", 0.0f);
            groundMat->setFloat("_Smoothness", 0.3f);
            groundRenderer->setMaterial(groundMat);
            
            // Standard material for comparison
            auto standardMat = BuiltinMaterials::createStandard();
            standardMat->setColor("_Color", color(0.8f, 0.2f, 0.9f));  // Purple
            standardMat->setFloat("_Metallic", 0.5f);
            standardMat->setFloat("_Smoothness", 0.8f);
            standardRenderer->setMaterial(standardMat);
            
            std::cout << "\n✓ Custom material system working!" << std::endl;
        });

    // Run engine
    Engine::runOpenGL(scene, 1280, 720, "Custom Material Demo", 60);

    return 0;
}
