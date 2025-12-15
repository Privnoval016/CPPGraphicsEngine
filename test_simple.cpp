#include "GraphicsEngine.h"

int main() {
    Scene scene;
    
    auto camera = scene.createGameObject("Camera");
    camera->transform.setPosition(vec3(0, 0, 5));
    camera->addComponent<CameraComponent>();
    
    auto cube = scene.createGameObject("Cube");
    cube->setMesh(Mesh::createCube());
    
    std::cout << "Objects: " << scene.getAllGameObjects().size() << std::endl;
    
    Camera* cam = nullptr;
    for (auto* obj : scene.getAllGameObjects()) {
        std::cout << obj->name << " - mesh: " << (obj->getMesh() != nullptr);
        if (auto* cc = obj->getComponent<CameraComponent>()) {
            cam = cc->getCamera();
            std::cout << " - camera found";
        }
        std::cout << std::endl;
    }
    
    std::cout << "Camera pointer: " << cam << std::endl;
    
    return 0;
}
