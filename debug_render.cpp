#include "GraphicsEngine.h"
#include <iostream>

int main() {
    Scene scene;
    scene.name = "Debug";
    
    // Create camera
    auto camera = scene.createGameObject("MainCamera");
    camera->transform.setPosition(vec3(0, 0, 10));
    auto* camComp = camera->addComponent<CameraComponent>();
    
    std::cout << "Camera created at: " << camera->transform.position() << std::endl;
    std::cout << "Camera component exists: " << (camComp != nullptr) << std::endl;
    
    // Create one cube
    auto cube = scene.createGameObject("TestCube");
    cube->transform.setPosition(vec3(0, 0, 0));
    cube->setMesh(Mesh::createCube());
    
    std::cout << "Cube created at: " << cube->transform.position() << std::endl;
    std::cout << "Cube has mesh: " << (cube->getMesh() != nullptr) << std::endl;
    std::cout << "Mesh vertex count: " << cube->getMesh()->vertices.size() << std::endl;
    
    // Check scene objects
    std::cout << "Total objects in scene: " << scene.getAllGameObjects().size() << std::endl;
    
    for (auto* obj : scene.getAllGameObjects()) {
        std::cout << "  - " << obj->name << " has mesh: " << (obj->getMesh() != nullptr) << std::endl;
        if (auto* cc = obj->getComponent<CameraComponent>()) {
            std::cout << "    Camera found! FOV: " << cc->getFOV() << std::endl;
        }
    }
    
    return 0;
}
