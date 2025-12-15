# CPP Graphics Engine

A 3D graphics engine built from scratch in C++ with a Unity-like component-based architecture.

## ✨ NEW: Real-Time Window Rendering!

The engine now supports **real-time rendering in a window** with camera controls!

```bash
./build/WindowDemo  # Opens a window with interactive 3D scene
```

**Controls**: WASD to move, Arrow keys to look, ESC to exit.

See [WINDOW_RENDERING.md](WINDOW_RENDERING.md) for full details!

---

## Features

### Rendering Backend
- ✅ **Software Rasterizer** - Custom triangle rasterization with no external graphics libraries
- ✅ **Real-Time Window Rendering** - SDL2-based live rendering with input controls
- ✅ **Depth Buffer** - Proper depth testing for 3D rendering
- ✅ **Perspective Projection** - Camera with configurable FOV, near/far planes
- ✅ **Wireframe & Solid Rendering** - Multiple render modes
- ✅ **Backface Culling** - Performance optimization
- ✅ **Barycentric Interpolation** - Smooth attribute interpolation across triangles

### Math Library
- ✅ **vec3** - 3D vector operations (dot, cross, normalize, etc.)
- ✅ **mat4** - 4x4 transformation matrices
- ✅ **Projection Matrices** - Perspective and orthographic
- ✅ **View Matrices** - LookAt camera transformations
- ✅ **Transform System** - Position, rotation, scale

### Lighting
- ✅ **Blinn-Phong Shading** - Ambient, diffuse, and specular components
- ✅ **Directional Lights** - Sun-like lighting
- ✅ **Point Lights** - Positional lights with attenuation
- ✅ **Multiple Lights** - Support for multiple light sources

### GameObject System (Unity-like)
- ✅ **GameObject** - Base entity class
- ✅ **Component System** - Attach behaviors to GameObjects
- ✅ **MonoBehaviour** - Script lifecycle (Awake, Start, Update, LateUpdate)
- ✅ **Transform Component** - All objects have transforms
- ✅ **Scene Management** - Organize GameObjects in scenes

### Built-in Meshes
- ✅ **Cube** - With proper normals and vertex colors
- ✅ **Sphere** - Icosphere with subdivision
- ✅ **Plane** - Ground plane for scenes

## Architecture

```
CPPGraphicsEngine/
├── Pose/               # Math library
│   ├── vec3.h         # 3D vectors
│   ├── mat4.h         # 4x4 matrices
│   └── transform.h    # Legacy transform (optional)
├── Rendering/         # Rendering pipeline
│   ├── camera.h       # Perspective camera
│   ├── mesh.h         # Mesh data structures
│   ├── framebuffer.h  # Color & depth buffers
│   ├── rasterizer.h   # Software rasterizer
│   ├── light.h        # Light types
│   └── color.h        # Color utilities
├── Core/              # Engine core
│   ├── gameEngine.h   # Main engine class
│   ├── scene.h        # Scene management
│   ├── gameObject.h   # GameObject class
│   ├── component.h    # Component base class
│   ├── monoBehaviour.h # Script base class
│   └── transformComponent.h # Transform component
└── Examples/          # Example scripts
    └── rotator.h      # Example MonoBehaviours
```

## Building

### Prerequisites

- C++20 compatible compiler
- CMake 3.30+
- SDL2 (for window rendering)

**Install SDL2**:
```bash
# macOS
brew install sdl2

# Ubuntu/Debian
sudo apt-get install libsdl2-dev

# Windows (vcpkg)
vcpkg install sdl2
```

### Build

```bash
mkdir build
cd build
cmake ..
make
```

This creates:
- `CPPGraphicsEngine` - File-based rendering
- `WindowDemo` - Real-time window rendering

## Usage

### Basic Scene Setup

```cpp
#include "Core/gameEngine.h"
#include "Core/scene.h"
#include "Examples/rotator.h"

int main()
{
    // Create engine
    GameEngine engine(800, 600);
    
    // Create scene
    Scene scene("My Scene");
    
    // Setup camera
    scene.mainCamera.position = vec3(0, 2, -8);
    scene.mainCamera.lookAt(vec3(0, 0, 0));
    
    // Add lighting
    scene.addLight(Light::directional(vec3(0, -1, 0), color(1, 1, 1), 1.0f));
    
    // Create a rotating cube
    GameObject* cube = scene.createGameObject("Cube");
    cube->setMesh(Mesh::createCube(1.0f));
    cube->addComponent<Rotator>()->rotationSpeed = vec3(0, 1, 0);
    
    // Run the engine
    engine.setActiveScene(&scene);
    engine.run(1); // Render 1 frame
    engine.saveFrame("output.ppm");
    
    return 0;
}
```

### Creating Custom MonoBehaviours

```cpp
class MyScript : public MonoBehaviour
{
public:
    float speed = 2.0f;
    
    void start() override
    {
        // Initialize
        std::cout << "MyScript started!\n";
    }
    
    void update(float deltaTime) override
    {
        // Rotate the object
        rotate(vec3(0, speed * deltaTime, 0));
        
        // Move forward
        translate(transform().forward() * speed * deltaTime);
    }
};

// Usage:
GameObject* obj = scene.createGameObject("MyObject");
obj->setMesh(Mesh::createCube());
auto script = obj->addComponent<MyScript>();
script->speed = 3.0f;
```

## Example Scripts

### Rotator
Rotates an object at a constant speed:
```cpp
auto rotator = obj->addComponent<Rotator>();
rotator->rotationSpeed = vec3(0, 1.0f, 0); // Rotate around Y-axis
```

### Oscillator
Makes an object move up and down:
```cpp
auto oscillator = obj->addComponent<Oscillator>();
oscillator->amplitude = 2.0f;
oscillator->frequency = 1.0f;
oscillator->axis = vec3(0, 1, 0);
```

### Orbiter
Makes an object orbit around a point:
```cpp
auto orbiter = obj->addComponent<Orbiter>();
orbiter->center = vec3::zero;
orbiter->radius = 5.0f;
orbiter->speed = 1.5f;
```

## Rendering Modes

```cpp
// Wireframe only
engine.rasterizer.renderMode = Rasterizer::RenderMode::Wireframe;

// Solid shading
engine.rasterizer.renderMode = Rasterizer::RenderMode::Solid;

// Both wireframe and solid
engine.rasterizer.renderMode = Rasterizer::RenderMode::SolidWireframe;

// Toggle backface culling
engine.rasterizer.backfaceCulling = true;
```

## Output

The engine renders to a framebuffer and can output:
- **PPM format** - Standard image format
- **Console output** - Direct to stdout
- **File output** - Save frames to disk

```cpp
// Save to file
engine.saveFrame("output.ppm");

// Output to console (can be piped)
engine.outputFrame();
```

## Performance

The engine uses:
- **Pure software rasterization** - No GPU dependencies
- **Optimized math operations** - Efficient linear algebra
- **Depth buffer optimization** - Early depth rejection
- **Backface culling** - Skip invisible triangles

For better performance, enable compiler optimizations:
```cmake
add_compile_options(-O3 -march=native -mtune=native)
```

## Future Enhancements

Potential additions:
- Texture mapping
- Normal mapping
- Shadow mapping
- Frustum culling
- Octree/BVH for scene optimization
- Multi-threading for rasterization
- More primitive shapes
- Material system
- Animation system
- Physics integration
- Input system
- Audio system

## License

This is a personal educational project. Feel free to learn from and modify the code.

## Demo

Run the included demo:
```bash
./build/CPPGraphicsEngine > output.ppm
```

This creates a scene with:
- A rotating cube with multiple colors
- An orbiting sphere
- An oscillating cube
- A ground plane
- Directional and point lighting

View the output:
```bash
# On macOS
open output.ppm

# On Linux
eog output.ppm

# On Windows
start output.ppm
```
