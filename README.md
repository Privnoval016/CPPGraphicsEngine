# CPP Graphics Engine

A lightweight 3D graphics engine with Unity-like architecture, written in C++ with hardware-accelerated OpenGL rendering.

## Quick Start

```cpp
#include "GraphicsEngine.h"
#include "Assets/Scripts/cameraController.h"

int main() {
    // Create scene
    Scene scene;
    scene.name = "MainScene";
    
    // Create camera
    auto camera = scene.createGameObject("MainCamera");
    camera->transform.setPosition(vec3(0, 5, 15));
    camera->addComponent<CameraComponent>();
    camera->addComponent<CameraController>();
    
    // Create player
    auto player = scene.createGameObject("Player");
    player->transform.setPosition(vec3(0, 1, 0));
    player->setMesh(Mesh::createCube());
    
    // Run engine
    Engine::runOpenGL(scene);
    
    return 0;
}
```

## Build & Run

```bash
mkdir build && cd build
cmake ..
make Game
./Game
```

## Project Structure (Unity-like)

```
CPPGraphicsEngine/
├── Assets/                    # Your game assets
│   ├── Scripts/              # Your game scripts
│   │   ├── cameraController.h
│   │   └── rotator.h
│   └── Scenes/               # Saved scenes (.scene files)
├── Engine/                    # Engine source (don't modify!)
│   ├── Core/                 # Component system, GameObject, Scene
│   ├── Math/                 # vec3, mat4
│   └── Rendering/            # Renderers, meshes, shaders
├── Docs/                      # Documentation
├── GraphicsEngine.h           # Single include header
├── main.cpp                   # Entry point (your game setup)
└── CMakeLists.txt             # Build configuration
```

## Architecture

### Component Hierarchy (Unity-style)

```
Component                    # Base - can attach to GameObject
    └── Behaviour            # Adds lifecycle + enable/disable  
            └── MonoBehaviour    # User scripts (inherit from this!)
```

### Transform Hierarchy

```cpp
// Parent-child relationships
auto parent = scene.createGameObject("Parent");
auto child = scene.createGameObject("Child");

child->transform.setParent(&parent->transform);
child->transform.setLocalPosition(vec3(0, 1, 0));  // 1 unit above parent

parent->transform.translate(vec3(5, 0, 0));
// Child automatically moves with parent!
```

### Lifecycle Methods

```cpp
class MyScript : public MonoBehaviour {
    void awake() override {
        // Called once when object created
    }
    
    void start() override {
        // Called once before first update
    }
    
    void update(float deltaTime) override {
        // Called every frame
    }
    
    void lateUpdate(float deltaTime) override {
        // Called after all updates (good for camera)
    }
    
    void onDestroy() override {
        // Called when object destroyed
    }
};
```

### Camera Component

```cpp
// Every camera needs CameraComponent
auto camera = scene.createGameObject("MainCamera");
camera->addComponent<CameraComponent>();
camera->addComponent<CameraController>();  // Optional controller

// Access camera
auto* camComp = camera->getComponent<CameraComponent>();
camComp->setFieldOfView(90.0f * M_PI / 180.0f);
camComp->setAspectRatio(16.0f / 9.0f);
```

### Scene Serialization

```cpp
// Save scene
SceneSerializer::saveSceneToAssets(scene, "Level1");
// Creates: Assets/Scenes/Level1.scene

// Load scene
Scene loadedScene = SceneSerializer::loadSceneFromAssets("Level1");
Engine::runOpenGL(loadedScene);
```

### Math (vec3)

```cpp
vec3 v(1, 2, 3);

// Member methods
v.length()            // Magnitude
v.lengthSquared()     // Faster than length()
v.normalized()        // Unit vector

// Static methods
vec3::dot(a, b)       // Dot product
vec3::cross(a, b)     // Cross product
vec3::distance(a, b)  // Distance between
vec3::lerp(a, b, t)   // Linear interpolation

// Direct member access
v.x, v.y, v.z         // Direct access
v[0], v[1], v[2]      // Array-style access

// Constants
vec3::zero, vec3::one
vec3::up, vec3::forward, vec3::right
```

## Features

✅ Hardware-accelerated OpenGL rendering (60+ FPS)  
✅ Unity-like component system (Component → Behaviour → MonoBehaviour)  
✅ Parent-child transform hierarchies  
✅ Camera component system  
✅ Scene save/load (.scene files)  
✅ Custom shader support  
✅ Built-in primitives (cube, sphere, plane)  
✅ Blinn-Phong lighting  
✅ Input system (keyboard + mouse)  
✅ Simple entry point (like Unity scenes)  

## Controls (Demo)

- **WASD** - Move camera
- **Space/Shift** - Up/Down
- **Right Mouse** - Look around
- **ESC** - Exit

## Creating Scripts

1. Create a new `.h` file in `Assets/Scripts/`
2. Inherit from `MonoBehaviour`
3. Override lifecycle methods

```cpp
// Assets/Scripts/enemy.h
#ifndef ENEMY_H
#define ENEMY_H

#include "../../Engine/Core/monoBehaviour.h"

class Enemy : public MonoBehaviour {
    float speed = 3.0f;
    
    void update(float deltaTime) override {
        // Move toward player
        rotate(vec3(0, 90 * deltaTime, 0));  // Spin
        translate(forward() * speed * deltaTime);  // Move forward
    }
};

#endif
```

4. Add to CMakeLists.txt USER_SCRIPTS section
5. Include in main.cpp and attach to GameObject:

```cpp
#include "Assets/Scripts/enemy.h"

auto enemy = scene.createGameObject("Enemy");
enemy->addComponent<Enemy>();
```

## Documentation

See `Docs/` folder for detailed guides:

- **ARCHITECTURE.md** - Component system explained
- **COMPONENT_REFERENCE.md** - Component vs MonoBehaviour
- **SHADER_GUIDE.md** - Custom shaders
- **GETTING_STARTED.md** - Full tutorial

## Requirements

- C++20 compiler
- CMake 3.30+
- SDL2
- OpenGL 4.1+ (Metal on macOS)

## License

MIT License - See LICENSE file

---

**Happy game dev! 🎮**
