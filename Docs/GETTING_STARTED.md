# Getting Started - 5 Minute Guide

## Quick Start

### 1. Build the Engine (30 seconds)

```bash
cd /path/to/CPPGraphicsEngine
mkdir build && cd build
cmake ..
make
```

### 2. Run the Demo (5 seconds)

```bash
./CPPGraphicsEngine > output.ppm
```

### 3. View the Result

```bash
open output.ppm  # macOS
eog output.ppm   # Linux
start output.ppm # Windows
```

You should see a 3D scene with rotating cubes, an orbiting sphere, and lighting!

## Your First Scene (5 minutes)

Create `my_first_scene.cpp`:

```cpp
#include "Core/gameEngine.h"
#include "Core/scene.h"
#include "Examples/rotator.h"

int main()
{
    // 1. Create engine (width, height)
    GameEngine engine(640, 480);
    
    // 2. Create scene
    Scene scene("My First Scene");
    
    // 3. Position camera
    scene.mainCamera.position = vec3(0, 2, -6);
    scene.mainCamera.lookAt(vec3(0, 0, 0));
    
    // 4. Add a light
    Light sun = Light::directional(
        vec3(0, -1, 0.5f),  // Direction
        color(1, 1, 1),      // White light
        1.0f                 // Full intensity
    );
    scene.addLight(sun);
    
    // 5. Create a spinning cube
    GameObject* cube = scene.createGameObject("MyCube");
    cube->setMesh(Mesh::createCube(2.0f));
    
    auto spinner = cube->addComponent<Rotator>();
    spinner->rotationSpeed = vec3(0.5f, 1.0f, 0);
    
    // 6. Create ground
    GameObject* ground = scene.createGameObject("Ground");
    ground->setMesh(Mesh::createPlane(10.0f, 10.0f));
    
    // 7. Run!
    engine.setActiveScene(&scene);
    engine.run(1);  // 1 frame
    engine.outputFrame();
    
    return 0;
}
```

Build and run:
```bash
# Add to CMakeLists.txt:
add_executable(my_scene my_first_scene.cpp)

# Build
cd build && make

# Run
./my_scene > my_output.ppm
```

## Create Your First Script (5 minutes)

Create `my_script.h`:

```cpp
#ifndef MY_SCRIPT_H
#define MY_SCRIPT_H

#include "Core/monoBehaviour.h"
#include <cmath>

class MyScript : public MonoBehaviour
{
public:
    float speed = 2.0f;
    float height = 1.0f;
    
private:
    float time = 0.0f;
    
public:
    void update(float deltaTime) override
    {
        time += deltaTime;
        
        // Bounce up and down
        float y = height + std::sin(time * speed) * 0.5f;
        setPosition(vec3(position().x(), y, position().z()));
        
        // Spin
        rotate(vec3(0, speed * deltaTime, 0));
    }
};

#endif
```

Use it:
```cpp
#include "my_script.h"

GameObject* obj = scene.createGameObject("BouncyCube");
obj->setMesh(Mesh::createCube());
auto script = obj->addComponent<MyScript>();
script->speed = 3.0f;
script->height = 2.0f;
```

## Common Tasks

### Change Camera Position
```cpp
scene.mainCamera.position = vec3(x, y, z);
scene.mainCamera.lookAt(vec3(0, 0, 0));
```

### Add More Lights
```cpp
// Sun
scene.addLight(Light::directional(vec3(0, -1, 0)));

// Lamp
scene.addLight(Light::point(vec3(5, 5, 5), color(1, 0.8f, 0.6f), 1.0f, 20.0f));
```

### Create Objects
```cpp
GameObject* obj = scene.createGameObject("MyObject");
obj->setMesh(Mesh::createCube());     // Cube
obj->setMesh(Mesh::createSphere());   // Sphere
obj->setMesh(Mesh::createPlane());    // Plane
```

### Position Objects
```cpp
obj->transform.position = vec3(1, 2, 3);
obj->transform.rotation = vec3(0, 3.14f/4, 0);  // 45° rotation
obj->transform.scale = vec3(2, 2, 2);           // 2x size
```

### Add Behaviors
```cpp
// Spin
obj->addComponent<Rotator>()->rotationSpeed = vec3(0, 1, 0);

// Bounce
auto osc = obj->addComponent<Oscillator>();
osc->amplitude = 2.0f;
osc->frequency = 1.0f;

// Orbit
auto orb = obj->addComponent<Orbiter>();
orb->center = vec3::zero;
orb->radius = 5.0f;
orb->speed = 1.0f;
```

### Render Settings
```cpp
// Wireframe
engine.rasterizer.renderMode = Rasterizer::RenderMode::Wireframe;

// Solid (default)
engine.rasterizer.renderMode = Rasterizer::RenderMode::Solid;

// Both
engine.rasterizer.renderMode = Rasterizer::RenderMode::SolidWireframe;
```

### Save Output
```cpp
engine.saveFrame("output.ppm");           // Save to file
engine.outputFrame();                     // Print to stdout
```

## Example Scenes

### Scene 1: Solar System
```cpp
// Sun
GameObject* sun = scene.createGameObject("Sun");
sun->setMesh(Mesh::createSphere(1.5f));
sun->addComponent<Rotator>()->rotationSpeed = vec3(0, 0.5f, 0);

// Planet orbiting sun
GameObject* planet = scene.createGameObject("Planet");
planet->setMesh(Mesh::createSphere(0.5f));
auto orbit = planet->addComponent<Orbiter>();
orbit->center = vec3::zero;
orbit->radius = 5.0f;
orbit->speed = 1.0f;
```

### Scene 2: Bouncing Cubes
```cpp
for (int i = 0; i < 5; i++)
{
    GameObject* cube = scene.createGameObject("Cube" + std::to_string(i));
    cube->setMesh(Mesh::createCube(0.8f));
    cube->transform.position = vec3(i * 2.0f - 4.0f, 1, 0);
    
    auto osc = cube->addComponent<Oscillator>();
    osc->frequency = 1.0f + i * 0.2f;
    osc->amplitude = 1.5f;
}
```

### Scene 3: Spinning Tower
```cpp
for (int i = 0; i < 8; i++)
{
    GameObject* cube = scene.createGameObject("Tower" + std::to_string(i));
    cube->setMesh(Mesh::createCube(1.0f));
    cube->transform.position = vec3(0, i * 1.2f, 0);
    
    auto rot = cube->addComponent<Rotator>();
    rot->rotationSpeed = vec3(0, 1.0f + i * 0.3f, 0);
}
```

## Troubleshooting

### "Build failed"
- Make sure C++20 is supported
- Check CMake version (3.30+)
- Verify all header files exist

### "Blank/black image"
- Check camera position (not inside objects)
- Verify objects are in view
- Add lighting (scene needs lights!)

### "Objects not visible"
- Camera might be too far/close
- Objects might be behind camera
- Check object positions

### "Image looks wrong"
- Verify normal calculations
- Check light directions
- Ensure depth buffering is working

## Next Steps

1. ✅ **You just created your first 3D scene!**
2. 📖 Read `QUICK_REFERENCE.md` for more API details
3. 🎮 Check `examples_advanced.cpp` for complex scenes
4. 🔧 Read `COMPONENT_GUIDE.md` to create custom scripts
5. 🏗️ Read `ARCHITECTURE.md` to understand the internals

## Quick Reference Card

```cpp
// Engine
GameEngine engine(width, height);
engine.setActiveScene(&scene);
engine.run(numFrames);

// Scene
Scene scene("Name");
scene.mainCamera.position = vec3(x, y, z);
scene.addLight(Light::directional(direction));

// GameObject
GameObject* obj = scene.createGameObject("Name");
obj->setMesh(mesh);
obj->transform.position = vec3(x, y, z);
obj->addComponent<ComponentType>();

// Component
class MyScript : public MonoBehaviour {
    void update(float dt) override { }
};

// Math
vec3 v(1, 2, 3);
float dot = dot(v1, v2);
vec3 cross = cross(v1, v2);
mat4 m = mat4::translation(vec3(1, 0, 0));
```

## Community & Help

- 📁 Check the `Examples/` folder for more scripts
- 📖 Read the documentation files for detailed info
- 🔍 Study `main.cpp` for a complete example
- 🧪 Experiment! Change values and see what happens

**Happy coding! You're now a graphics engine developer! 🎨🚀**
