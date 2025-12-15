# Getting Started

Quick guide to building and using the engine.

## Installation

### Prerequisites

- **C++ Compiler** - GCC 9+, Clang 10+, or MSVC 2019+
- **CMake** - Version 3.10 or higher
- **SDL2** - For window and input handling
- **OpenGL** - Version 4.1 or higher (usually built-in on modern systems)

### Install SDL2

**macOS:**
```bash
brew install sdl2
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt install libsdl2-dev
```

**Windows:**
Download SDL2 development libraries from [libsdl.org](https://www.libsdl.org/)

### Build

```bash
git clone <repo-url>
cd CPPGraphicsEngine
mkdir build && cd build
cmake ..
make
```

This creates two executables:
- `Game` - Main demo (51 objects)
- `MaterialDemo` - Material system showcase (19 objects)

## Your First Project

### 1. Create Your Game File

Create `my_game.cpp`:

```cpp
#include "GraphicsEngine.h"
#include "Assets/Scripts/cameraController.h"

int main()
{
    // Create scene
    Scene scene;
    scene.name = "MyGame";
    
    // Create camera
    auto camera = scene.createGameObject("Camera");
    camera->transform.setPosition(vec3(0, 5, 15));
    camera->addComponent<CameraComponent>();
    camera->addComponent<CameraController>();
    
    // Create a spinning cube
    auto cube = scene.createGameObject("Cube");
    cube->transform.setPosition(vec3(0, 1, 0));
    cube->addComponent<MeshFilter>()->setMesh(Mesh::createCube());
    cube->addComponent<MeshRenderer>();
    
    // Run engine
    Engine::runOpenGL(scene, 1280, 720, "My Game");
    
    return 0;
}
```

### 2. Add to Build System

Edit `CMakeLists.txt`, add before the last line:

```cmake
# Your game
add_executable(MyGame my_game.cpp)
target_include_directories(MyGame PRIVATE ${CMAKE_SOURCE_DIR})
```

### 3. Build and Run

```bash
cd build
make MyGame
./MyGame
```

## Creating Custom Scripts

### 1. Create Script File

Create `Assets/Scripts/myScript.h`:

```cpp
#ifndef MY_SCRIPT_H
#define MY_SCRIPT_H

#include "Engine/Core/Components/monoBehaviour.h"
#include "Engine/Core/Systems/input.h"

class MyScript : public MonoBehaviour
{
public:
    float speed = 5.0f;
    
    void start() override
    {
        // Called once before first update
        std::cout << "MyScript started on " << gameObject->name << std::endl;
    }
    
    void update(float deltaTime) override
    {
        // Move with WASD
        vec3 movement(0, 0, 0);
        
        if (Input::getKey(SDLK_w)) movement.z -= speed * deltaTime;
        if (Input::getKey(SDLK_s)) movement.z += speed * deltaTime;
        if (Input::getKey(SDLK_a)) movement.x -= speed * deltaTime;
        if (Input::getKey(SDLK_d)) movement.x += speed * deltaTime;
        
        gameObject->transform.translate(movement);
        
        // Jump
        if (Input::getKeyDown(SDLK_SPACE)) {
            std::cout << "Jump!" << std::endl;
        }
    }
};

#endif
```

### 2. Use Your Script

In your game file:

```cpp
#include "Assets/Scripts/myScript.h"

// Add to GameObject
auto player = scene.createGameObject("Player");
player->addComponent<MyScript>();
```

## Using Materials

Materials must be created **after** OpenGL initializes. Use the callback:

```cpp
Engine::runOpenGL(scene, 1280, 720, "Game", 60, 
    [&](Scene& s) {
        // Create materials here
        auto redMaterial = BuiltinMaterials::createStandard();
        redMaterial->setColor("_Color", color(1, 0, 0));
        redMaterial->setFloat("_Metallic", 0.8f);
        redMaterial->setFloat("_Smoothness", 0.9f);
        
        // Apply to object
        cube->getComponent<MeshRenderer>()->setMaterial(redMaterial);
    });
```

### Material Types

**Standard (PBR):**
```cpp
auto mat = BuiltinMaterials::createStandard();
mat->setColor("_Color", color(r, g, b));
mat->setFloat("_Metallic", 0.0f);      // 0=non-metal, 1=metal
mat->setFloat("_Smoothness", 0.5f);    // 0=rough, 1=smooth
```

**Unlit (no lighting):**
```cpp
auto mat = BuiltinMaterials::createUnlit();
mat->setColor("_Color", color(r, g, b));
```

**Standard Specular:**
```cpp
auto mat = BuiltinMaterials::createStandardSpecular();
mat->setColor("_Color", color(r, g, b));
mat->setColor("_SpecColor", color(1, 1, 1));
mat->setFloat("_Smoothness", 0.8f);
```

## Scene Hierarchy

Create parent-child relationships:

```cpp
auto parent = scene.createGameObject("Parent");
auto child = scene.createGameObject("Child");

child->transform.setParent(&parent->transform);
child->transform.setLocalPosition(vec3(0, 2, 0));  // 2 units above parent

// Moving parent moves children
parent->transform.translate(vec3(5, 0, 0));
```

## Saving/Loading Scenes

```cpp
// Save
SceneSerializer::saveToFile(scene, "Assets/Scenes/level1.scene");

// Load
Scene loaded = SceneSerializer::loadFromFile("Assets/Scenes/level1.scene");
Engine::runOpenGL(loaded);
```

## Common Patterns

### Access Components

```cpp
// Get component (returns nullptr if not found)
auto renderer = gameObject->getComponent<MeshRenderer>();
if (renderer) {
    renderer->setEnabled(false);
}

// Get from other objects
auto player = scene.findGameObject("Player");
if (player) {
    auto script = player->getComponent<MyScript>();
}
```

### Find Objects

```cpp
// By name
auto obj = scene.findGameObject("Player");

// All objects
for (auto* obj : scene.getAllGameObjects()) {
    std::cout << obj->name << std::endl;
}
```

### Spawn Objects at Runtime

```cpp
void MyScript::update(float deltaTime) {
    if (Input::getKeyDown(SDLK_SPACE)) {
        auto newObj = gameObject->scene->createGameObject("Spawned");
        newObj->transform.setPosition(gameObject->transform.getPosition());
        newObj->addComponent<MeshFilter>()->setMesh(Mesh::createCube());
        newObj->addComponent<MeshRenderer>();
    }
}
```

### Frame-Rate Independent Movement

Always use `deltaTime` for smooth movement:

```cpp
void update(float deltaTime) override {
    float speed = 5.0f;
    gameObject->transform.translate(vec3(0, 0, -speed * deltaTime));
}
```

## Controls (Built-in CameraController)

- **WASD** - Move forward/back/left/right
- **Space** - Move up
- **Shift** - Move down
- **Mouse** - Look around
- **Left Ctrl** - Sprint (move faster)
- **ESC** - Exit application

## Next Steps

- See [Architecture](ARCHITECTURE.md) for detailed system explanation
- Check `main.cpp` and `material_demo.cpp` for complete examples
- Create your own scripts in `Assets/Scripts/`
- Experiment with materials and lighting

## Troubleshooting

**"SDL2 not found"**
- Make sure SDL2 is installed (`brew install sdl2` on macOS)
- On Linux: `sudo apt install libsdl2-dev`

**"No OpenGL context"**
- Your system must support OpenGL 4.1+
- Update graphics drivers

**"Materials crash"**
- Materials must be created in the `onOpenGLReady` callback
- Don't create materials before `Engine::runOpenGL()` is called

**Objects not rendering**
- Make sure GameObject has both `MeshFilter` and `MeshRenderer` components
- Check that camera has `CameraComponent`
- Verify objects are in camera's view frustum
