//
// Model Test - Test loading AmongUs.obj model
//

#include "../GraphicsEngine.h"
#include "../Assets/Scripts/rotator.h"
#include "../Assets/Scripts/cameraController.h"

int main()
{
    Scene scene;
    scene.name = "ModelTest";
    scene.backgroundColor = color(0.1f, 0.1f, 0.15f);

    std::cout << "\n=== Model Loading Test ===" << std::endl;
    
    // Create camera
    auto camera = scene.createGameObject("MainCamera");
    camera->transform.setPosition(vec3(0, 2, 5));
    camera->transform.setRotation(vec3(0, M_PI, 0));
    camera->addComponent<CameraComponent>();
    camera->addComponent<CameraController>();

    // Create ground
    auto ground = scene.createGameObject("Ground");
    ground->addComponent<MeshFilter>()->setMesh(Mesh::createPlane(20, 20));
    auto groundRenderer = ground->addComponent<MeshRenderer>();

    // Load AmongUs model
    std::cout << "\nLoading AmongUs.obj..." << std::endl;
    auto amongUsMesh = ModelLoader::loadFromFile("Assets/Models/AmongUs.obj");
    
    if (amongUsMesh) {
        std::cout << "Successfully loaded AmongUs model!" << std::endl;
        
        auto amongUs = scene.createGameObject("AmongUs");
        amongUs->transform.setPosition(vec3(0, 0, 0));
        amongUs->transform.setScale(vec3(0.5f, 0.5f, 0.5f));  // Scale down if needed
        amongUs->addComponent<MeshFilter>()->setMesh(amongUsMesh);
        auto amongUsRenderer = amongUs->addComponent<MeshRenderer>();
        amongUs->addComponent<Rotator>()->rotationSpeed = vec3(0, 0.5f, 0);
        
        std::cout << "Scene objects: " << scene.getAllGameObjects().size() << std::endl;
    } else {
        std::cerr << "Failed to load AmongUs model!" << std::endl;
        return 1;
    }

    std::cout << "\nControls:" << std::endl;
    std::cout << "  WASD - Move" << std::endl;
    std::cout << "  Mouse - Look" << std::endl;
    std::cout << "  Space/Shift - Up/Down" << std::endl;
    std::cout << "  ESC - Exit\n" << std::endl;

    // Set up materials after OpenGL initialization
    scene.onOpenGLReady([&](Scene& s) {
        // Ground material
        auto groundMat = BuiltinMaterials::createStandard();
        groundMat->setColor("_Color", color(0.3f, 0.3f, 0.3f));
        groundMat->setFloat("_Metallic", 0.0f);
        groundMat->setFloat("_Smoothness", 0.4f);
        groundRenderer->setMaterial(groundMat);
        
        // Load texture and create material for AmongUs
        std::cout << "\nLoading AmongUs texture..." << std::endl;
        auto amongUsTexture = TextureLoader::loadFromFile("Assets/Textures/AmongUs.png");
        
        auto amongUsMat = BuiltinMaterials::createStandard();
        if (amongUsTexture) {
            amongUsMat->setTexture("_MainTex", amongUsTexture);
            // Flag is set automatically by setTexture()
            std::cout << "✓ Texture applied to material" << std::endl;
        } else {
            // Fallback to solid color if texture fails
            std::cout << "Texture failed, using gold material instead" << std::endl;
            amongUsMat = MaterialSerializer::loadFromFile("Assets/Materials/gold.mat");
        }
        
        auto amongUs = s.findGameObject("AmongUs");
        if (amongUs && amongUsMat) {
            amongUs->getComponent<MeshRenderer>()->setMaterial(amongUsMat);
        }
    });

    // Run engine
    Engine::runOpenGL(scene, 1280, 720, "Model Test - AmongUs", 60);

    return 0;
}
