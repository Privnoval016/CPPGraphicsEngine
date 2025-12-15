# Unity-like Architecture Reference

## Directory Structure

```
CPPGraphicsEngine/
├── Assets/                    # YOUR game content (like Unity)
│   ├── Scripts/              # Your C++ scripts
│   │   ├── cameraController.h
│   │   └── rotator.h
│   └── Scenes/               # Saved scenes (.scene files)
│
├── Engine/                    # Engine code (like Unity's internals)
│   ├── Core/                 # Don't modify
│   ├── Math/
│   └── Rendering/
│
├── main.cpp                   # Entry point (your game setup)
├── GraphicsEngine.h           # Single include
├── CMakeLists.txt
└── README.md
```

## Entry Point (main.cpp)

**Unity**: You create scenes in the editor and press Play
**This Engine**: You create scenes in `main.cpp` and run `./Game`

```cpp
#include "GraphicsEngine.h"
#include "Assets/Scripts/cameraController.h"

int main() {
    // 1. Create scene (like Unity scene)
    Scene scene;
    scene.name = "MainScene";
    
    // 2. Add GameObjects with components
    auto camera = scene.createGameObject("MainCamera");
    camera->addComponent<CameraComponent>();
    camera->addComponent<CameraController>();
    
    auto player = scene.createGameObject("Player");
    player->setMesh(Mesh::createCube());
    
    // 3. Run (like Unity's Play button)
    Engine::runOpenGL(scene);
    
    return 0;
}
```

## Component System

**Unity**: `MonoBehaviour` scripts
**This Engine**: Same! Inherit from `MonoBehaviour`

```cpp
// Assets/Scripts/playerController.h
#include "../../Engine/Core/monoBehaviour.h"

class PlayerController : public MonoBehaviour {
    float speed = 5.0f;
    
    void start() override {
        // Called once
    }
    
    void update(float deltaTime) override {
        // Every frame
        if (Input::getKey(SDL_SCANCODE_W)) {
            translate(vec3(0, 0, -speed * deltaTime));
        }
    }
};
```

## Camera System

**Unity**: Camera component on GameObject
**This Engine**: Same!

```cpp
// Create camera GameObject
auto camera = scene.createGameObject("MainCamera");

// Add CameraComponent (required)
camera->addComponent<CameraComponent>();

// Add controller (optional)
camera->addComponent<CameraController>();

// Position
camera->transform.setPosition(vec3(0, 5, 15));
```

## Scene Management

**Unity**: Save scenes in editor
**This Engine**: Save/load programmatically

```cpp
// Save scene
SceneSerializer::saveSceneToAssets(scene, "Level1");
// → Assets/Scenes/Level1.scene

// Load scene
Scene level1 = SceneSerializer::loadSceneFromAssets("Level1");
Engine::runOpenGL(level1);
```

## Hierarchy

**Unity**: Parent-child in hierarchy window
**This Engine**: `setParent()`

```cpp
auto parent = scene.createGameObject("Parent");
auto child = scene.createGameObject("Child");

// Make child follow parent
child->transform.setParent(&parent->transform);
child->transform.setLocalPosition(vec3(0, 1, 0));

// Move parent → child moves too
parent->transform.translate(vec3(5, 0, 0));
```

## Workflow Comparison

### Unity Workflow
1. Open Unity Editor
2. Create scene visually
3. Drag/drop components
4. Write C# scripts
5. Press Play

### This Engine Workflow
1. Open main.cpp
2. Create scene in code
3. Add components in code
4. Write C++ scripts in Assets/Scripts/
5. Build and run: `make Game && ./Game`

## Key Differences from Unity

| Feature | Unity | This Engine |
|---------|-------|-------------|
| Scene Creation | Visual editor | Code in main.cpp |
| Scripts Location | Any subfolder in Assets | Assets/Scripts/ |
| Script Language | C# | C++ |
| Component Attach | Drag-drop | `addComponent<T>()` |
| Build | Build button | `make Game` |
| Play | Play button | `./Game` |
| Camera | Camera component | CameraComponent |
| Serialization | Automatic | Manual save/load |

## Adding New Scripts

### 1. Create Script File
```cpp
// Assets/Scripts/myScript.h
#ifndef MY_SCRIPT_H
#define MY_SCRIPT_H

#include "../../Engine/Core/monoBehaviour.h"

class MyScript : public MonoBehaviour {
    void update(float deltaTime) override {
        // Your code
    }
};

#endif
```

### 2. Add to CMakeLists.txt
```cmake
set(USER_SCRIPTS
    Assets/Scripts/rotator.h
    Assets/Scripts/cameraController.h
    Assets/Scripts/myScript.h  # Add this
)
```

### 3. Include in main.cpp
```cpp
#include "Assets/Scripts/myScript.h"

// Use it
auto obj = scene.createGameObject("MyObject");
obj->addComponent<MyScript>();
```

### 4. Rebuild
```bash
cd build
make Game
./Game
```

## Best Practices

### ✅ Do
- Keep all game scripts in `Assets/Scripts/`
- Use `main.cpp` only for scene setup
- Add components to control behavior
- Use parent-child hierarchies
- Save scenes for level design

### ❌ Don't
- Modify anything in `Engine/` folder
- Put scripts outside `Assets/Scripts/`
- Write game logic in `main.cpp`
- Forget to add scripts to CMakeLists.txt
- Mix rendering code with game logic

## Quick Commands

```bash
# Clean build
rm -rf build && mkdir build && cd build && cmake .. && make Game

# Run
./build/Game

# Or (from build/)
make Game && ./Game
```

## Summary

This engine mimics Unity's architecture:
- **Assets/** = Your game content
- **Engine/** = Unity's internal code
- **main.cpp** = Scene setup (like Unity scenes)
- **MonoBehaviour** = Script base class
- **CameraComponent** = Camera system
- **SceneSerializer** = Save/load scenes

Focus on `Assets/Scripts/` and `main.cpp` - the engine handles everything else!
