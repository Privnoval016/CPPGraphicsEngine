# 3D Graphics Engine - Architecture Overview

## Project Structure

```
CPPGraphicsEngine/
│
├── Core/                      # Engine core systems
│   ├── gameEngine.h          # Main engine class (GameEngine)
│   ├── scene.h               # Scene management (Scene)
│   ├── gameObject.h          # Entity class (GameObject)
│   ├── component.h           # Component base class
│   ├── monoBehaviour.h       # Script base class
│   └── transformComponent.h  # Position/rotation/scale
│
├── Pose/                      # Linear algebra & math
│   ├── vec3.h                # 3D vectors with operations
│   ├── mat4.h                # 4x4 matrices for transformations
│   └── transform.h           # Legacy transform (optional)
│
├── Rendering/                 # Graphics pipeline
│   ├── camera.h              # Perspective camera
│   ├── mesh.h                # Mesh data (vertices, triangles)
│   ├── framebuffer.h         # Color & depth buffers
│   ├── rasterizer.h          # Software rasterization
│   ├── light.h               # Lighting system
│   └── color.h               # Color utilities
│
├── Examples/                  # Example MonoBehaviours
│   └── rotator.h             # Rotator, Oscillator, Orbiter
│
├── main.cpp                   # Demo application
├── examples_simple.cpp        # Simple example
├── examples_advanced.cpp      # Advanced example
│
└── Documentation/
    ├── README.md             # Main documentation
    ├── COMPONENT_GUIDE.md    # Component system guide
    └── QUICK_REFERENCE.md    # Quick reference
```

## System Architecture

### 1. Math Layer (Pose/)

**Purpose**: Foundation for all 3D calculations

**Components**:
- `vec3`: 3D vectors with dot product, cross product, normalization
- `mat4`: 4x4 matrices for transformations, projections, view matrices
- Transform helpers: translation, rotation, scaling matrices

**Key Features**:
- Pure mathematical operations
- No dependencies on other systems
- Optimized for common graphics operations

### 2. Rendering Pipeline (Rendering/)

**Flow**:
```
Scene Setup → Transformation → Rasterization → Framebuffer → Output
```

**Components**:

#### Camera
- Perspective projection
- View matrix calculation
- Position and orientation in 3D space
- FOV, aspect ratio, near/far planes

#### Mesh
- Vertex data (position, normal, UV, color)
- Triangle indices
- Procedural generation (cube, sphere, plane)
- Normal calculation

#### Framebuffer
- 2D array of pixels (color buffer)
- Depth buffer for visibility testing
- Clear operations
- PPM image output

#### Rasterizer
- Triangle rasterization using barycentric coordinates
- Wireframe rendering (Bresenham's line algorithm)
- Depth testing
- Backface culling
- Attribute interpolation

#### Light
- Directional lights (sun-like)
- Point lights (positional with attenuation)
- Spot lights (cone-shaped)
- Blinn-Phong shading model

**Rendering Process**:
1. Transform vertices to world space (model matrix)
2. Transform to view space (view matrix)
3. Project to clip space (projection matrix)
4. Convert to screen space (viewport transform)
5. Rasterize triangles
6. Apply lighting calculations
7. Write to framebuffer with depth testing

### 3. Core Engine (Core/)

**Architecture**: Entity-Component System

#### GameObject
- Container for components
- Has a transform (position, rotation, scale)
- Can have a mesh for rendering
- Manages component lifecycle

#### Component
- Base class for all attachable behaviors
- Lifecycle methods: awake, start, update, lateUpdate, onDestroy
- Reference to owning GameObject

#### MonoBehaviour
- Specialized component for user scripts
- Convenient helper methods
- Similar to Unity's MonoBehaviour

#### Scene
- Container for GameObjects
- Main camera
- Lighting setup
- Background color
- Rendering orchestration

#### GameEngine
- Main engine class
- Manages active scene
- Frame timing (deltaTime)
- Rendering loop
- Output generation

**Engine Loop**:
```
Initialize
    ↓
Awake (all components)
    ↓
Start (all components)
    ↓
┌─────────────────┐
│ Frame Loop      │
│   Update()      │ ← Game logic
│   LateUpdate()  │ ← Post-processing
│   Render()      │ ← Draw frame
└─────────────────┘
```

## Data Flow

### Transform Pipeline
```
Local Transform → Model Matrix → View Matrix → Projection Matrix → Screen Space
```

### Component Communication
```
GameObject
    ├── TransformComponent (always present)
    ├── Mesh (optional)
    └── Components[]
            ├── Component A (can access GameObject)
            ├── Component B (can access other components)
            └── Component C
```

### Rendering Pipeline
```
Scene
    └── GameObjects[]
            ├── Transform → Model Matrix
            ├── Mesh → Vertices & Triangles
            └── Components (update transforms)
                    ↓
            Camera → View & Projection Matrices
                    ↓
            Rasterizer
                    ├── Transform vertices
                    ├── Clip & cull
                    ├── Rasterize triangles
                    └── Apply lighting
                    ↓
            Framebuffer
                    ├── Color buffer
                    └── Depth buffer
                    ↓
            Output (PPM file or stdout)
```

## Key Design Decisions

### 1. Software Rasterization
**Why**: No external graphics libraries (OpenGL, DirectX), maximum portability
**Trade-off**: Slower than GPU, but educational and controllable

### 2. Component-Based Architecture
**Why**: Flexible, modular, Unity-like API familiar to game developers
**Benefits**: 
- Easy to extend
- Components are reusable
- Clean separation of concerns

### 3. Header-Only Libraries
**Why**: Simplicity, no linking issues
**Trade-off**: Longer compile times, but easier to use

### 4. C++20 Standard
**Why**: Modern C++ features (concepts, ranges potential)
**Current Usage**: Mostly C++17 features with C++20 enabled

### 5. PPM Output Format
**Why**: Simplest image format, no library needed
**Trade-off**: Large file sizes, but easy to implement and debug

## Performance Characteristics

### Bottlenecks
1. **Rasterization**: O(n × pixels) where n = number of triangles
2. **Depth Testing**: Per-pixel operation
3. **Transform Calculations**: Per-vertex operations
4. **Lighting**: Per-pixel for each light source

### Optimizations Implemented
- ✅ Backface culling (skip ~50% of triangles)
- ✅ Bounding box for rasterization (test fewer pixels)
- ✅ Early depth rejection
- ✅ Component caching (avoid repeated lookups)

### Future Optimizations
- Spatial data structures (octree, BVH)
- Frustum culling
- Multi-threading for rasterization
- SIMD for vector operations
- Tile-based rendering

## Extension Points

### Adding New Components
```cpp
class MyComponent : public MonoBehaviour
{
public:
    void update(float dt) override { /* your logic */ }
};
```

### Adding New Mesh Types
```cpp
static std::shared_ptr<Mesh> createCustomMesh()
{
    auto mesh = std::make_shared<Mesh>();
    // Add vertices and triangles
    return mesh;
}
```

### Adding New Light Types
Extend the `Light` class with new types in `light.h`

### Adding Render Passes
Extend the `Rasterizer` class to add:
- Shadow mapping
- Post-processing effects
- Multiple render targets

### Adding Physics
Create physics components that update transforms:
```cpp
class RigidBody : public Component
{
    vec3 velocity;
    void update(float dt) override
    {
        gameObject->transform.translate(velocity * dt);
    }
};
```

## Testing Strategy

### Unit Tests
- Math operations (vec3, mat4)
- Matrix transformations
- Mesh generation

### Integration Tests
- Complete rendering pipeline
- Component lifecycle
- Scene management

### Visual Tests
- Render known scenes
- Compare output images
- Check for artifacts

## Build System

### CMake Configuration
- C++20 standard
- Header-only structure
- Optional optimization flags
- Platform-agnostic

### Compilation
```bash
mkdir build && cd build
cmake ..
make
```

### Optimization Flags
```cmake
add_compile_options(-O3 -march=native)
```

## Memory Management

### Smart Pointers
- `std::shared_ptr` for shared ownership (Mesh, Components)
- Raw pointers for references (GameObject*, Component*)

### Lifetime Rules
- Scene owns GameObjects
- GameObjects own Components
- Meshes are shared between GameObjects

### Cleanup
- Automatic via destructors
- `onDestroy()` for custom cleanup

## Future Roadmap

### Near-term (Easy)
- [ ] Material system
- [ ] Texture mapping
- [ ] More primitive shapes
- [ ] Input system
- [ ] Time management utilities

### Mid-term (Moderate)
- [ ] Scene serialization (save/load)
- [ ] Animation system
- [ ] Particle system
- [ ] Audio system
- [ ] Physics integration

### Long-term (Complex)
- [ ] Editor UI
- [ ] Visual scripting
- [ ] Asset pipeline
- [ ] Networking
- [ ] Advanced lighting (shadows, GI)

## Comparison to Unity

| Feature | This Engine | Unity |
|---------|-------------|-------|
| Language | C++ | C# + C++ |
| Rendering | Software | Hardware (GPU) |
| Editor | None | Full editor |
| Components | ✅ | ✅ |
| MonoBehaviour | ✅ | ✅ |
| Scene System | ✅ | ✅ |
| Physics | ❌ | ✅ |
| Asset Pipeline | ❌ | ✅ |
| Size | ~3K LOC | Millions LOC |

## Learning Resources

### Understanding the Code
1. Start with `main.cpp` - see a complete example
2. Read `gameEngine.h` - understand the engine loop
3. Explore `scene.h` - see how objects are managed
4. Study `rasterizer.h` - learn the rendering pipeline
5. Check `monoBehaviour.h` - understand components

### Key Algorithms
- **Barycentric Coordinates**: Triangle rasterization
- **Bresenham's Algorithm**: Line drawing
- **Blinn-Phong**: Lighting model
- **LookAt Matrix**: Camera positioning
- **Perspective Projection**: 3D to 2D conversion

### Recommended Reading
- "Computer Graphics: Principles and Practice" - Foley et al.
- "Real-Time Rendering" - Akenine-Möller et al.
- "Game Engine Architecture" - Gregory
- "Mathematics for 3D Game Programming" - Lengyel

## Contributing

### Code Style
- CamelCase for classes: `GameObject`, `MonoBehaviour`
- camelCase for methods: `update()`, `getComponent()`
- lowercase for files: `gameObject.h`

### Adding Features
1. Create header in appropriate folder
2. Document public API
3. Add to CMakeLists.txt
4. Create example usage
5. Update documentation

### Debugging Tips
- Use `std::clog` for debug output (goes to stderr)
- Visual debugging: render wireframes
- Print matrices to verify transformations
- Check depth buffer values

## Contact & Support

This is an educational project. For questions:
- Read the documentation
- Check the examples
- Examine the source code
- Experiment and learn!

---

**Version**: 1.0  
**Last Updated**: 2025  
**Lines of Code**: ~3000  
**Dependencies**: C++ Standard Library only  
**License**: Educational / Personal Use
