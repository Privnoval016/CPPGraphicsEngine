# File Audit Report
**Date**: Generated during codebase cleanup
**Purpose**: Identify unused files and clarify architecture

## ❌ Files to DELETE (Unused)

### 1. `Pose/transform.h`
- **Size**: 300+ lines
- **Purpose**: Advanced transform system with parent-child hierarchies, RotateAngleAxis, lookAt, etc.
- **Usage**: NEVER included in any source file (only listed in CMakeLists.txt)
- **Reason**: Complete elaborate system built but never integrated
- **Action**: DELETE - functionality not used by engine

### 2. `test.cpp`
- **Purpose**: Test file from development
- **Usage**: Never compiled or used
- **Action**: DELETE

### 3. `includes.h`
- **Purpose**: Unknown, likely early development artifact
- **Usage**: Never included anywhere
- **Action**: DELETE

### 4. `image.ppm`
- **Purpose**: Output image from software renderer
- **Usage**: Generated output, not source code
- **Action**: DELETE (can be regenerated)

## ✅ Files to KEEP (Active)

### Math System (`Pose/`)
- ✅ `vec3.h` - 3D vector math (used in 13+ files)
- ✅ `mat4.h` - 4x4 matrix transformations (used in 12+ files)
- ❌ `transform.h` - DELETE (unused)

### Core Engine (`Core/`)
- ✅ `component.h` - Base component class
- ✅ `monoBehaviour.h` - User script base with helpers
- ✅ `transformComponent.h` - Simple transform (86 lines, ACTUALLY USED)
- ✅ `gameObject.h` - Scene object container
- ✅ `scene.h` - Scene management
- ✅ `gameEngine.h` - Main engine loop
- ✅ `input.h` - Input handling

### Rendering System (`Rendering/`)
- ✅ `color.h` - Color representation
- ✅ `mesh.h` - Mesh data structures
- ✅ `camera.h` - Camera view/projection
- ✅ `light.h` - Lighting system
- ✅ `framebuffer.h` - Software rendering buffer
- ✅ `rasterizer.h` - Software rasterizer
- ✅ `window.h` - SDL software rendering window
- ✅ `opengl_window.h` - OpenGL window
- ✅ `opengl_renderer.h` - Hardware renderer
- ✅ `shader.h` - Shader compilation system
- ✅ `default_shaders.h` - Built-in shaders

### Examples (`Examples/`)
- ✅ `rotator.h` - Example MonoBehaviour scripts

### Demo Programs
- ✅ `main.cpp` - Software renderer demo
- ✅ `window_demo.cpp` - Real-time software rendering
- ✅ `opengl_demo.cpp` - Hardware accelerated demo

### Documentation
- ✅ All `.md` files - Keep for reference

### Build System
- ✅ `CMakeLists.txt` - Build configuration (needs update to remove transform.h)
- ✅ `build/` - Build artifacts (can rebuild)
- ✅ `cmake-build-debug/` - IDE build artifacts

## 📋 Architecture Clarifications

### Component vs MonoBehaviour

**Component** (`Core/component.h`):
```cpp
class Component {
    GameObject* gameObject;
    bool enabled;
    
    virtual void awake() {}
    virtual void start() {}
    virtual void update(float deltaTime) {}
    virtual void lateUpdate(float deltaTime) {}
    virtual void onDestroy() {}
};
```
- **Purpose**: Base class for **data-only** components
- **Use Case**: Health systems, collision data, stats
- **No helpers**: Direct component implementation

**MonoBehaviour** (`Core/monoBehaviour.h`):
```cpp
class MonoBehaviour : public Component {
    // Convenience helpers:
    TransformComponent& transform() { return gameObject->transform; }
    vec3 position() const { return gameObject->transform.position; }
    void setPosition(const vec3& pos) { ... }
    void rotate(const vec3& angles) { ... }
    // ... more helpers
};
```
- **Purpose**: Base class for **user scripts** with convenience methods
- **Use Case**: Player controllers, enemies, game logic
- **Helper methods**: Quick access to transform, position, rotation

**Why both?**
- Component: Minimal overhead for data components
- MonoBehaviour: Ergonomic API for gameplay scripts (like Unity)

### transform.h vs transformComponent.h

**`Pose/transform.h`** ❌ UNUSED:
- 300+ lines with parent-child hierarchies
- Features: RotateAngleAxis, lookAt, rotateAround, dirty flags
- NEVER included in any source file
- Built but never integrated

**`Core/transformComponent.h`** ✅ USED:
- 86 lines, simple and functional
- Features: position, rotation, scale, getModelMatrix()
- **No parent hierarchies** (not implemented in actual engine)
- This is what GameObjects actually use

**Recommendation**: 
- DELETE `transform.h` (dead code)
- If parent hierarchies needed, implement them in `transformComponent.h`

## 📊 Summary

| Category | Files | Action |
|----------|-------|--------|
| To Delete | 4 | transform.h, test.cpp, includes.h, image.ppm |
| Active Source | 25+ | Keep all in Core/, Rendering/, Pose/ (except transform.h) |
| Documentation | 10+ | Keep all .md files |
| Build Artifacts | Many | Safe to delete (rebuild with cmake) |

## 🔧 Action Items

1. ✅ Delete unused files
2. ✅ Update CMakeLists.txt to remove transform.h references
3. ✅ Fix ground plane visibility bug
4. ⚠️ Consider: Implement parent hierarchies in transformComponent if needed
