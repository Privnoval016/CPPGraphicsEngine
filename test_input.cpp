#include "GraphicsEngine.h"

class InputTester : public MonoBehaviour {
public:
    void update(float deltaTime) override {
        Input& input = Input::getInstance();
        
        if (input.getKey(SDL_SCANCODE_W)) {
            std::cout << "W pressed!" << std::endl;
        }
        if (input.getKey(SDL_SCANCODE_SPACE)) {
            std::cout << "SPACE pressed!" << std::endl;
        }
    }
};

int main() {
    Scene scene;
    
    auto testObj = scene.createGameObject("Tester");
    testObj->addComponent<InputTester>();
    
    auto camera = scene.createGameObject("Camera");
    camera->transform.setPosition(vec3(0, 0, 5));
    camera->addComponent<CameraComponent>();
    
    Engine::runOpenGL(scene, 800, 600, "Input Test", 60);
    return 0;
}
