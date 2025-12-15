# Real-Time Window Rendering Guide

## Overview

The engine now supports **real-time rendering in a window** using SDL2! You can now see your 3D scenes running live with camera controls and interactive input.

## Building with Window Support

### Prerequisites

Install SDL2 on your system:

**macOS (Homebrew)**:
```bash
brew install sdl2
```

**Ubuntu/Debian**:
```bash
sudo apt-get install libsdl2-dev
```

**Windows (vcpkg)**:
```bash
vcpkg install sdl2
```

### Building

```bash
mkdir build && cd build
cmake ..
make
```

This creates two executables:
- `CPPGraphicsEngine` - Original (renders to file)
- `WindowDemo` - New real-time window rendering

## Running the Window Demo

```bash
./build/WindowDemo
```

A window will open showing your 3D scene rendering in real-time!

### Controls

- **WASD** - Move camera forward/back/left/right
- **Arrow Keys** - Rotate camera view
- **Space** - Move camera up
- **Shift** - Move camera down
- **ESC** - Exit

## Creating Window-Enabled Applications

### Basic Window App

```cpp
#include "Core/gameEngine.h"
#include "Core/scene.h"

int main()
{
    // Create engine with window (third parameter = true)
    GameEngine engine(1024, 768, true);
    
    // Create your scene
    Scene scene("My Scene");
    scene.mainCamera.position = vec3(0, 2, -8);
    scene.addLight(Light::directional(vec3(0, -1, 0)));
    
    // Add objects
    GameObject* cube = scene.createGameObject("Cube");
    cube->setMesh(Mesh::createCube());
    
    // Set scene and run
    engine.setActiveScene(&scene);
    engine.runInteractive();  // Real-time loop!
    
    return 0;
}
```

### Adding Camera Controls

```cpp
#include "Core/input.h"

class CameraController : public MonoBehaviour
{
public:
    Camera* camera;
    float speed = 5.0f;
    
    void update(float deltaTime) override
    {
        Input& input = Input::getInstance();
        
        vec3 movement(0, 0, 0);
        if (input.getKey(Input::W))
            movement += camera->forward;
        if (input.getKey(Input::S))
            movement -= camera->forward;
        if (input.getKey(Input::A))
            movement -= camera->right;
        if (input.getKey(Input::D))
            movement += camera->right;
            
        camera->position += normalize(movement) * speed * deltaTime;
    }
};

// In main:
GameObject* camObj = scene.createGameObject("CameraController");
auto controller = camObj->addComponent<CameraController>();
controller->camera = &scene.mainCamera;
```

## Input System

The engine includes a complete input system accessible via `Input::getInstance()`.

### Keyboard Input

```cpp
Input& input = Input::getInstance();

// Check if key is currently pressed
if (input.getKey(Input::W)) { /* W is held */ }

// Check if key was just pressed this frame
if (input.getKeyDown(Input::SPACE)) { /* Space just pressed */ }

// Check if key was just released this frame
if (input.getKeyUp(Input::ESC)) { /* ESC just released */ }
```

### Available Key Constants

```cpp
Input::W, Input::A, Input::S, Input::D
Input::SPACE, Input::SHIFT, Input::CTRL, Input::ESC
Input::UP, Input::DOWN, Input::LEFT, Input::RIGHT
```

### Mouse Input

```cpp
Input& input = Input::getInstance();

// Mouse position
int x = input.getMouseX();
int y = input.getMouseY();

// Mouse movement
int deltaX = input.getMouseDeltaX();
int deltaY = input.getMouseDeltaY();

// Mouse buttons
if (input.getMouseButton(SDL_BUTTON_LEFT)) { /* Left button held */ }
if (input.getMouseButtonDown(SDL_BUTTON_RIGHT)) { /* Right just clicked */ }
```

## Performance

Real-time rendering performance depends on:
- **Resolution**: Lower resolution = higher FPS
- **Scene complexity**: Fewer triangles = higher FPS
- **Lighting**: Fewer lights = higher FPS

### Typical Performance

| Resolution | Triangles | FPS (Apple M4) |
|------------|-----------|----------------|
| 640×480    | ~1000     | ~30-60 FPS     |
| 800×600    | ~1000     | ~20-40 FPS     |
| 1024×768   | ~1000     | ~15-30 FPS     |
| 1280×720   | ~1000     | ~10-20 FPS     |

### Optimization Tips

1. **Lower resolution**: Start with 640×480 for smoother rendering
2. **Reduce geometry**: Use simpler meshes
3. **Fewer lights**: 1-2 lights is usually enough
4. **Backface culling**: Already enabled by default
5. **Wireframe mode**: Faster than solid rendering

## Comparison: File vs Window Rendering

### File Rendering (Original)

```cpp
GameEngine engine(800, 600, false);  // No window
engine.setActiveScene(&scene);
engine.run(1);  // Render 1 frame
engine.saveFrame("output.ppm");
```

**Use cases**:
- Generating images
- Animation sequences
- High-quality renders
- Batch processing

### Window Rendering (New!)

```cpp
GameEngine engine(800, 600, true);  // With window!
engine.setActiveScene(&scene);
engine.runInteractive();  // Real-time loop
```

**Use cases**:
- Interactive demos
- Game development
- Real-time visualization
- Testing and debugging

## Advanced: Custom Update Loop

For more control, you can write your own loop:

```cpp
GameEngine engine(800, 600, true);
engine.setActiveScene(&scene);
engine.initialize();

while (engine.window->isOpen)
{
    // Poll events
    if (!engine.window->pollEvents())
        break;
    
    // Update input
    Input::getInstance().update();
    
    // Your custom logic here
    float dt = 1.0f / 60.0f;  // Or calculate actual deltaTime
    engine.update(dt);
    engine.render();
    
    // Display
    auto pixels = engine.framebuffer.getPixelData();
    engine.window->display(pixels.data());
}
```

## Example Scripts Using Input

### First Person Camera

```cpp
class FirstPersonCamera : public MonoBehaviour
{
public:
    Camera* camera;
    float moveSpeed = 5.0f;
    float lookSpeed = 2.0f;
    float pitch = 0.0f, yaw = 0.0f;
    
    void update(float dt) override
    {
        Input& input = Input::getInstance();
        
        // Movement
        vec3 move(0, 0, 0);
        if (input.getKey(Input::W)) move += camera->forward;
        if (input.getKey(Input::S)) move -= camera->forward;
        if (input.getKey(Input::A)) move -= camera->right;
        if (input.getKey(Input::D)) move += camera->right;
        
        if (move.sqr_magnitude() > 0)
            camera->position += normalize(move) * moveSpeed * dt;
        
        // Look with arrow keys
        if (input.getKey(Input::UP)) pitch += lookSpeed * dt;
        if (input.getKey(Input::DOWN)) pitch -= lookSpeed * dt;
        if (input.getKey(Input::LEFT)) yaw += lookSpeed * dt;
        if (input.getKey(Input::RIGHT)) yaw -= lookSpeed * dt;
        
        camera->rotation = vec3(pitch, yaw, 0);
    }
};
```

### Object Spawner

```cpp
class Spawner : public MonoBehaviour
{
private:
    Scene* scene;
    
public:
    void update(float dt) override
    {
        Input& input = Input::getInstance();
        
        if (input.getKeyDown(Input::SPACE))
        {
            // Spawn a cube
            GameObject* cube = scene->createGameObject("Spawned");
            cube->setMesh(Mesh::createCube());
            cube->transform.position = position() + vec3(0, 2, 0);
            cube->addComponent<Rotator>();
        }
    }
};
```

## Troubleshooting

### "Window not available"
- Make sure you pass `true` to GameEngine constructor
- Check that SDL2 is properly installed

### Low FPS
- Reduce window resolution
- Simplify scene geometry
- Use fewer lights
- Enable backface culling (default)

### Window doesn't open
- Check SDL2 installation: `brew list sdl2`
- Check CMake found SDL2: look for "Found SDL2" in cmake output

### Input not working
- Make sure to call `Input::getInstance().update()` each frame
- The engine does this automatically in `runInteractive()`

## Complete Example

See `window_demo.cpp` for a complete working example with:
- Real-time window rendering
- Camera controls (WASD + arrows)
- Multiple animated objects
- Interactive movement
- FPS display in title bar

Run it with:
```bash
./build/WindowDemo
```

## Next Steps

Now that you have real-time rendering:
1. Add mouse look camera controls
2. Create interactive objects
3. Build a simple game
4. Add physics interactions
5. Implement collision detection

**You now have a fully interactive 3D graphics engine!** 🎮✨
