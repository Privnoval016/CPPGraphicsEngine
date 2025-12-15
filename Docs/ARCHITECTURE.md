# Architecture Overview

How the engine is structured and how everything works together.

## Project Structure

```
CPPGraphicsEngine/
├── Assets/                      # Your game content
│   ├── Scripts/                 # Your custom MonoBehaviour scripts
│   └── Scenes/                  # Saved scene files (.scene)
│
├── Engine/                      # Engine source code (don't modify!)
│   ├── Core/                    # Component system & game loop
│   │   ├── gameObject.h         # Entity with components
│   │   ├── scene.h              # Container for GameObjects
│   │   ├── Components/          # Built-in components
│   │   │   ├── component.h      # Base component class
│   │   │   ├── behaviour.h      # Lifecycle + enable/disable
│   │   │   ├── monoBehaviour.h  # User script base class
│   │   │   ├── transformComponent.h
│   │   │   ├── cameraComponent.h
│   │   │   ├── meshFilter.h     # Holds mesh data
│   │   │   └── meshRenderer.h   # Renders mesh
│   │   └── Systems/
│   │       ├── input.h          # Keyboard & mouse input
│   │       └── sceneSerializer.h
│   │
│   ├── Math/                    # Linear algebra
│   │   ├── vec3.h               # 3D vectors
│   │   └── mat4.h               # 4x4 matrices
│   │
│   └── Rendering/               # Graphics pipeline
│       ├── camera.h             # Camera & projection
│       ├── mesh.h               # Mesh primitives
│       ├── material.h           # Material system
│       ├── texture.h            # Texture handling
│       ├── builtin_materials.h  # PBR materials
│       ├── opengl_renderer.h    # OpenGL backend
│       ├── opengl_window.h      # SDL window
│       └── Shaders/
│           ├── shader.h         # Shader program
│           └── default_shaders.h
│
├── GraphicsEngine.h             # Single include for users
├── main.cpp                     # Your game entry point
├── material_demo.cpp            # Material system demo
└── CMakeLists.txt               # Build configuration
```

## Component System (Unity-like)

### Hierarchy

```
Component                    Base class for all components
    ├── TransformComponent   Position, rotation, scale
    ├── CameraComponent      Renders from GameObject's perspective
    ├── MeshFilter           Holds mesh data
    ├── MeshRenderer         Renders the mesh
    └── Behaviour            Adds lifecycle methods
            └── MonoBehaviour    Your custom scripts inherit this
```

### GameObject

A GameObject is a container for components. It always has a Transform.

```cpp
// Create GameObject
auto obj = scene.createGameObject("MyObject");

// Add components
auto meshFilter = obj->addComponent<MeshFilter>();
auto renderer = obj->addComponent<MeshRenderer>();
auto script = obj->addComponent<MyCustomScript>();

// Get components
auto filter = obj->getComponent<MeshFilter>();

// Transform (every GameObject has one)
obj->transform.setPosition(vec3(0, 1, 0));
obj->transform.rotate(vec3(0, M_PI/2, 0));
```

### Component Lifecycle

```cpp
class MyScript : public MonoBehaviour {
    void awake() override {
        // Called once when GameObject is created
        // Use for initialization
    }
    
    void start() override {
        // Called once before first update
        // Use for setup that requires other components
    }
    
    void update(float deltaTime) override {
        // Called every frame
        // Main game logic here
    }
    
    void lateUpdate(float deltaTime) override {
        // Called after all updates
        // Use for camera follow, etc.
    }
    
    void onEnable() override {
        // Called when enabled
    }
    
    void onDisable() override {
        // Called when disabled
    }
};
```

## Rendering Pipeline

### Flow

```
1. Scene Update
   └─> All component update() methods called

2. Find Camera
   └─> Get CameraComponent from scene

3. For Each GameObject:
   └─> Get MeshRenderer component
       └─> Get MeshFilter component
           └─> Upload mesh to GPU
               └─> Apply material (if set)
                   └─> Draw mesh
```

### Materials

Materials define how objects are rendered. They contain:
- **Shader** - GPU program (vertex + fragment shaders)
- **Properties** - Colors, floats, textures passed to shader

```cpp
auto material = BuiltinMaterials::createStandard();
material->setColor("_Color", color(1, 0, 0));
material->setFloat("_Metallic", 0.8f);
material->setFloat("_Smoothness", 0.9f);

renderer->setMaterial(material);
```

**Built-in Shaders:**
- **Standard (PBR)** - Physically-based rendering with Cook-Torrance BRDF
  - Metallic/Roughness workflow
  - Supports normal maps, metallic maps, AO maps
- **Unlit** - No lighting calculations, just color/texture
- **Standard Specular** - PBR with specular color instead of metallic

### Transform Hierarchy

Transforms can have parent-child relationships:

```cpp
auto parent = scene.createGameObject("Parent");
auto child = scene.createGameObject("Child");

// Make child follow parent
child->transform.setParent(&parent->transform);

// Local vs World coordinates
child->transform.setLocalPosition(vec3(0, 2, 0));  // 2 units above parent
vec3 worldPos = child->transform.getPosition();    // World position

// Moving parent moves all children
parent->transform.translate(vec3(5, 0, 0));
```

## Engine Loop

```cpp
Engine::runOpenGL(scene, width, height, title, fps, onOpenGLReady);
```

**Steps:**
1. **Initialize OpenGL** - Create window, context, renderer
2. **Call onOpenGLReady** - User callback for creating materials
3. **Scene.awake()** - Call awake() on all components
4. **Scene.start()** - Call start() on all components
5. **Main Loop:**
   - Poll input events
   - Update Input system
   - Scene.update(deltaTime) - Call update() on all components
   - Scene.lateUpdate(deltaTime) - Call lateUpdate() on all components
   - Render scene
   - Swap buffers
6. **Cleanup** - Destroy window and context

## Math System

### vec3 - 3D Vectors

```cpp
vec3 v(x, y, z);
v.length()           // Magnitude
v.normalized()       // Unit vector
vec3::dot(a, b)     // Dot product
vec3::cross(a, b)   // Cross product
v + u, v - u        // Vector arithmetic
v * scalar          // Scalar multiplication
```

### mat4 - 4x4 Matrices

```cpp
mat4::identity()                        // Identity matrix
mat4::translate(x, y, z)                // Translation matrix
mat4::rotateX/Y/Z(angle)                // Rotation matrices
mat4::scale(x, y, z)                    // Scale matrix
mat4::perspective(fov, aspect, near, far) // Projection matrix
mat4::lookAt(eye, target, up)           // View matrix

mat4 result = A * B;                    // Matrix multiplication
vec3 transformed = M * v;               // Transform vector
```

### Transform

Combines position, rotation, scale into transformation matrices:

```cpp
transform.setPosition(vec3(0, 1, 0));
transform.setRotation(vec3(0, M_PI, 0));  // Euler angles
transform.setScale(vec3(2, 2, 2));

mat4 modelMatrix = transform.getModelMatrix();
```

## Input System

Singleton that tracks keyboard and mouse state:

```cpp
Input& input = Input::getInstance();

// Keyboard
bool held = Input::getKey(SDLK_w);
bool pressed = Input::getKeyDown(SDLK_SPACE);
bool released = Input::getKeyUp(SDLK_SPACE);

// Mouse
bool mouseHeld = Input::getMouseButton(SDL_BUTTON_LEFT);
vec2 mousePos = Input::getMousePosition();
vec2 mouseDelta = Input::getMouseDelta();  // Movement since last frame
```

## Scene Management

### Creating Objects at Runtime

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

### Saving/Loading Scenes

```cpp
// Save
SceneSerializer::saveToFile(scene, "Assets/Scenes/level1.scene");

// Load
Scene loadedScene = SceneSerializer::loadFromFile("Assets/Scenes/level1.scene");
Engine::runOpenGL(loadedScene);
```

## Best Practices

### Do:
- ✅ Inherit from `MonoBehaviour` for game scripts
- ✅ Use `addComponent<>()` to add functionality
- ✅ Create materials in `onOpenGLReady` callback
- ✅ Use `deltaTime` for frame-rate independent movement
- ✅ Check for nullptr when using `getComponent<>()`

### Don't:
- ❌ Modify files in `Engine/` directory
- ❌ Create materials before OpenGL initialization
- ❌ Hardcode frame rates (use deltaTime)
- ❌ Create GameObject without adding to Scene
- ❌ Mix vertex colors with material colors (use materials only)

## Performance Tips

- Use object pooling for frequently spawned objects
- Minimize component lookups (cache references in `start()`)
- Use `lateUpdate()` for camera operations
- Batch similar objects with same material
- Keep mesh vertex counts reasonable (<10k vertices per mesh)
