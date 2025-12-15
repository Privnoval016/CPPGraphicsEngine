# CPP Graphics Engine Documentation

A Unity-like 3D graphics engine written in C++ with OpenGL rendering.

## Table of Contents

1. [Getting Started](GETTING_STARTED.md) - Installation, setup, and first project
2. [Architecture](ARCHITECTURE.md) - Project structure and how everything works

## Quick Reference

### Creating a Scene

```cpp
#include "GraphicsEngine.h"

Scene scene;
scene.name = "MyScene";

// Add camera
auto camera = scene.createGameObject("Camera");
camera->transform.setPosition(vec3(0, 5, 15));
camera->addComponent<CameraComponent>();

// Add objects
auto cube = scene.createGameObject("Cube");
cube->addComponent<MeshFilter>()->setMesh(Mesh::createCube());
cube->addComponent<MeshRenderer>();

// Run
Engine::runOpenGL(scene, 1280, 720, "My Game");
```

### Components

**Core Components:**
- `CameraComponent` - Renders the scene from this GameObject's perspective
- `MeshFilter` - Holds mesh data (vertices, triangles)
- `MeshRenderer` - Renders the mesh with optional material

**Creating Custom Scripts:**

```cpp
class MyScript : public MonoBehaviour {
    void awake() override { /* Called once on creation */ }
    void start() override { /* Called before first update */ }
    void update(float deltaTime) override { /* Called every frame */ }
    void lateUpdate(float deltaTime) override { /* Called after all updates */ }
};

// Usage:
gameObject->addComponent<MyScript>();
```

### Materials

Materials control how objects are rendered. Create them after OpenGL initialization:

```cpp
Engine::runOpenGL(scene, 1280, 720, "Game", 60, 
    [&](Scene& s) {
        // OpenGL is ready - create materials here
        auto material = BuiltinMaterials::createStandard();
        material->setColor("_Color", color(1, 0, 0));  // Red
        material->setFloat("_Metallic", 0.8f);
        material->setFloat("_Smoothness", 0.9f);
        
        myObject->getComponent<MeshRenderer>()->setMaterial(material);
    });
```

**Built-in Materials:**
- `BuiltinMaterials::createStandard()` - PBR with metallic/roughness workflow
- `BuiltinMaterials::createUnlit()` - No lighting, just color/texture
- `BuiltinMaterials::createStandardSpecular()` - PBR with specular workflow

**Material Properties:**
- `setColor(name, color)` - Set color property
- `setFloat(name, value)` - Set float property
- `setTexture(name, texture)` - Set texture property

**Standard Material Properties:**
- `_Color` - Base color (default: white)
- `_Metallic` - 0.0 (dielectric) to 1.0 (metal)
- `_Smoothness` - 0.0 (rough) to 1.0 (smooth)
- `_MainTex` - Albedo texture (optional)

### Input

```cpp
// Keyboard
if (Input::getKey(SDLK_w)) { /* W is held */ }
if (Input::getKeyDown(SDLK_SPACE)) { /* Space just pressed */ }
if (Input::getKeyUp(SDLK_SPACE)) { /* Space just released */ }

// Mouse
if (Input::getMouseButton(SDL_BUTTON_LEFT)) { /* Left mouse held */ }
vec2 mousePos = Input::getMousePosition();
vec2 mouseDelta = Input::getMouseDelta();
```

### Transform

```cpp
// Position
transform.setPosition(vec3(x, y, z));
transform.translate(vec3(dx, dy, dz));
vec3 pos = transform.getPosition();

// Rotation (Euler angles in radians)
transform.setRotation(vec3(pitch, yaw, roll));
transform.rotate(vec3(dpitch, dyaw, droll));
vec3 rot = transform.getRotation();

// Scale
transform.setScale(vec3(sx, sy, sz));
vec3 scale = transform.getScale();

// Directions
vec3 forward = transform.forward();
vec3 right = transform.right();
vec3 up = transform.up();

// Hierarchy
transform.setParent(parentTransform);
auto children = transform.getChildren();
```

### Built-in Meshes

```cpp
Mesh::createCube()           // 1x1x1 cube
Mesh::createSphere(r, subdiv) // Sphere with radius and subdivisions
Mesh::createPlane(w, h)      // Flat plane
```

### Scene Management

```cpp
// Create objects
auto obj = scene.createGameObject("Name");

// Find objects
auto obj = scene.findGameObject("Name");
auto objs = scene.getAllGameObjects();

// Save/Load
SceneSerializer::saveToFile(scene, "scene.scene");
Scene loadedScene = SceneSerializer::loadFromFile("scene.scene");
```

## Features

- **Hardware-Accelerated Rendering** - OpenGL 4.1 with modern shaders
- **PBR Materials** - Physically-based rendering with Cook-Torrance BRDF
- **Component System** - Unity-like GameObject + Component architecture
- **Transform Hierarchy** - Parent-child relationships with local/world transforms
- **Input System** - Keyboard and mouse input handling
- **Scene Management** - Save/load scenes to files
- **Built-in Primitives** - Cubes, spheres, planes
- **Camera Controls** - First-person camera controller included

## Controls (Default)

- **WASD** - Move camera
- **Mouse** - Look around
- **Space** - Move up
- **Shift** - Move down
- **Ctrl** - Sprint (faster movement)
- **ESC** - Exit

## Examples

See `main.cpp` and `material_demo.cpp` for complete examples.
