# Quick Reference - Updated Features

## Custom Shaders (NEW!)

### Load and Use Custom Shader
```cpp
#include "Rendering/shader.h"
#include "Rendering/opengl_renderer.h"

auto myShader = std::make_shared<Shader>();
myShader->compileFromFile("vertex.glsl", "fragment.glsl");
renderer.setShader(myShader);
```

### Set Shader Uniforms
```cpp
shader->use();
shader->setFloat("time", currentTime);
shader->setVec3("cameraPos", camera.position);
shader->setMat4("modelMatrix", transform);
shader->setColor("tintColor", color(1, 0.5, 0));
```

## Configuration (NEW!)

### Modify Engine Settings
Edit `Core/engine_config.h`:

```cpp
// Increase max lights
constexpr int MAX_LIGHTS = 16;

// Change camera defaults
constexpr float DEFAULT_FOV = 90.0f;
constexpr float DEFAULT_MOVE_SPEED = 10.0f;

// Debug wireframe
constexpr bool WIREFRAME_MODE = true;
```

## Available Shaders

### Blinn-Phong (Default)
```cpp
// Automatically loaded on initialize()
renderer.initialize();
```

### Unlit (No Lighting)
```cpp
auto unlitShader = std::make_shared<Shader>();
unlitShader->compileFromSource(
    DefaultShaders::UNLIT_VERTEX,
    DefaultShaders::UNLIT_FRAGMENT
);
renderer.setShader(unlitShader);
```

### Custom Toon Shader
See `SHADER_GUIDE.md` for complete example

## Fixed Issues

### Ground Plane Visible
The ground plane now renders correctly:
```cpp
auto ground = Mesh::createPlane(20, 20);
// Plane uses proper CCW winding order
```

### Transform Parent Hierarchies
```cpp
// Child follows parent
child.setParent(&parent);
parent.translate(vec3(5, 0, 0));  // Child moves too
```

### Transform Utilities
```cpp
// Rotate around arbitrary axis
transform.rotateAngleAxis(M_PI/4, vec3(0, 1, 0));

// Look at target
transform.lookAt(targetPosition);

// Get direction vectors
vec3 fwd = transform.forward();
vec3 right = transform.right();
vec3 up = transform.up();
```

## File Organization

### Cleaned Up
- ❌ Removed: `Updating/` folder (old code)
- ❌ Removed: `test.cpp`, `examples_*.cpp`
- ❌ Removed: 500+ lines of unused code

### New Files
- ✅ `Rendering/shader.h` - Shader system
- ✅ `Rendering/default_shaders.h` - Built-in shaders
- ✅ `Core/engine_config.h` - Configuration constants
- ✅ `SHADER_GUIDE.md` - Custom shader tutorial
- ✅ `CLEANUP_SUMMARY.md` - What changed

## Performance

| Feature | Performance |
|---------|-------------|
| OpenGL Renderer | 60+ FPS |
| Software Renderer | 10-15 FPS |
| Max Lights | 8 (configurable) |
| Mesh Caching | Automatic |
| Shader Caching | Automatic |

## Quick Start

### Run the Demo
```bash
cd build
./OpenGLDemo
```

### Controls
- **WASD** - Move camera
- **Hold Right Mouse** - Look around
- **Space/Shift** - Move up/down
- **Left Ctrl** - Speed boost
- **ESC** - Exit

### Create Your Own Scene
```cpp
#include "Rendering/opengl_window.h"
#include "Rendering/opengl_renderer.h"

int main() {
    OpenGLWindow window(1280, 720);
    OpenGLRenderer renderer;
    renderer.initialize();
    
    Camera camera(vec3(0, 5, 10), vec3(0, 0, 0));
    camera.setPerspective(70.0f * M_PI/180, 16.0f/9.0f, 0.1f, 100.0f);
    
    auto mesh = Mesh::createCube();
    mat4 model = mat4::translation(vec3(0, 0, 0));
    
    std::vector<Light> lights;
    lights.push_back(Light::directional(vec3(-1, -1, -1)));
    
    while (window.isOpen) {
        window.pollEvents();
        renderer.clear();
        renderer.drawMesh(*mesh, model, camera, lights);
        window.swapBuffers();
    }
    
    return 0;
}
```

## Documentation

| Guide | Purpose |
|-------|---------|
| `README.md` | Overview and features |
| `SHADER_GUIDE.md` | Custom shader tutorial |
| `PERFORMANCE_AND_FEATURES.md` | Performance tips |
| `CLEANUP_SUMMARY.md` | What changed |
| `ARCHITECTURE.md` | System design |
| `WINDOW_RENDERING.md` | Real-time rendering |

## Common Tasks

### Write Custom Shader
1. Create `.glsl` files or R"(...)" strings
2. Compile with `Shader::compileFromSource()` or `compileFromFile()`
3. Set to renderer with `renderer.setShader(shader)`
4. Set uniforms before drawing

### Add More Lights
1. Edit `MAX_LIGHTS` in `Core/engine_config.h`
2. Update shader uniform array size
3. Rebuild

### Change Visual Style
1. Modify default shaders in `Rendering/default_shaders.h`
2. Or create custom shader (see `SHADER_GUIDE.md`)
3. Adjust lighting constants in `engine_config.h`

### Debug Rendering Issues
1. Enable debug mode: `DEBUG_MODE = true` in `engine_config.h`
2. Enable wireframe: `WIREFRAME_MODE = true`
3. Check shader compilation errors in console
4. Verify all uniforms are set

## What's Next?

Try these enhancements:
1. **Textures** - Add UV coordinates and texture sampling
2. **Shadows** - Implement shadow mapping
3. **Post-Processing** - Add blur, bloom, tone mapping
4. **Particles** - Create particle system
5. **Physics** - Add collision detection

See code comments and documentation for implementation details!
