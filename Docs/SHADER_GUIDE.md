# Custom Shader Guide

## Overview

The engine now supports custom GLSL shaders! You can write your own vertex and fragment shaders to create custom rendering effects.

## Quick Start

### Using Default Shaders

The engine comes with built-in Blinn-Phong shaders:

```cpp
#include "Rendering/opengl_renderer.h"
#include "Rendering/default_shaders.h"

OpenGLRenderer renderer;
renderer.initialize();  // Automatically uses Blinn-Phong shader
```

### Creating a Custom Shader

```cpp
#include "Rendering/shader.h"

// Create shader
auto myShader = std::make_shared<Shader>();

// Compile from source strings
const char* vertexSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 2) in vec3 aColor;
    
    out vec3 Color;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    void main() {
        Color = aColor;
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

const char* fragmentSource = R"(
    #version 330 core
    in vec3 Color;
    out vec4 FragColor;
    
    void main() {
        FragColor = vec4(Color, 1.0);
    }
)";

if (myShader->compileFromSource(vertexSource, fragmentSource))
{
    renderer.setShader(myShader);
}
```

### Loading Shaders from Files

```cpp
auto myShader = std::make_shared<Shader>();
if (myShader->compileFromFile("shaders/my_vertex.glsl", "shaders/my_fragment.glsl"))
{
    renderer.setShader(myShader);
}
```

## Built-in Shaders

The engine provides several pre-made shaders in `default_shaders.h`:

### 1. Blinn-Phong (Default)
- **Vertex**: `DefaultShaders::BLINN_PHONG_VERTEX`
- **Fragment**: `DefaultShaders::BLINN_PHONG_FRAGMENT`
- **Features**: Full lighting with ambient, diffuse, and specular
- **Uniforms Required**:
  - `mat4 model, view, projection`
  - `vec3 viewPos`
  - `Light lights[8]`
  - `int numLights`

### 2. Unlit
- **Vertex**: `DefaultShaders::UNLIT_VERTEX`
- **Fragment**: `DefaultShaders::UNLIT_FRAGMENT`
- **Features**: No lighting, just vertex colors
- **Uniforms Required**: `mat4 model, view, projection`

### 3. Wireframe
- **Vertex**: Uses `UNLIT_VERTEX`
- **Fragment**: `DefaultShaders::WIREFRAME_FRAGMENT`
- **Features**: Solid color output
- **Uniforms Required**: `mat4 model, view, projection`, `vec3 wireframeColor`

## Vertex Attribute Layout

Meshes use the following vertex layout:

| Location | Attribute | Type | Description |
|----------|-----------|------|-------------|
| 0 | Position | vec3 | Vertex position in local space |
| 1 | Normal | vec3 | Vertex normal for lighting |
| 2 | Color | vec3 | RGB vertex color |

## Setting Shader Uniforms

The `Shader` class provides convenient setters:

```cpp
shader->use();  // Activate shader first

// Basic types
shader->setInt("myInt", 42);
shader->setFloat("myFloat", 3.14f);
shader->setBool("myBool", true);

// Vectors
shader->setVec3("myVec", vec3(1, 0, 0));
shader->setVec3("myVec", 1.0f, 0.0f, 0.0f);  // Alternative

// Colors
shader->setColor("myColor", color(1, 0.5f, 0));

// Matrices
shader->setMat4("myMatrix", modelMatrix);
shader->setMat4("myMatrix", modelMatrix, false);  // No transpose
```

## Example: Custom Toon Shader

```cpp
const char* toonVertex = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    
    out vec3 Normal;
    out vec3 FragPos;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    void main() {
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

const char* toonFragment = R"(
    #version 330 core
    in vec3 Normal;
    in vec3 FragPos;
    out vec4 FragColor;
    
    uniform vec3 viewPos;
    uniform vec3 lightDir;
    uniform vec3 baseColor;
    
    void main() {
        vec3 norm = normalize(Normal);
        vec3 lightDirection = normalize(-lightDir);
        
        // Calculate diffuse
        float diff = max(dot(norm, lightDirection), 0.0);
        
        // Quantize to 3 levels (toon shading)
        float toonDiff;
        if (diff > 0.95)
            toonDiff = 1.0;
        else if (diff > 0.5)
            toonDiff = 0.6;
        else if (diff > 0.25)
            toonDiff = 0.4;
        else
            toonDiff = 0.2;
        
        vec3 result = baseColor * toonDiff;
        FragColor = vec4(result, 1.0);
    }
)";

// Use it
auto toonShader = std::make_shared<Shader>();
toonShader->compileFromSource(toonVertex, toonFragment);

// In render loop
toonShader->use();
toonShader->setVec3("viewPos", camera.position);
toonShader->setVec3("lightDir", vec3(-1, -1, -1));
toonShader->setColor("baseColor", color(0.2f, 0.6f, 1.0f));
// ... set matrices ...

renderer.setShader(toonShader);
renderer.drawMesh(mesh, modelMatrix, camera, lights);
```

## Example: Rim Lighting

```cpp
const char* rimFragment = R"(
    #version 330 core
    in vec3 Normal;
    in vec3 FragPos;
    in vec3 VertexColor;
    out vec4 FragColor;
    
    uniform vec3 viewPos;
    uniform vec3 rimColor;
    uniform float rimPower;
    
    void main() {
        vec3 norm = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos);
        
        // Rim lighting (Fresnel-like effect)
        float rim = 1.0 - max(dot(viewDir, norm), 0.0);
        rim = pow(rim, rimPower);
        
        vec3 result = VertexColor + rimColor * rim;
        FragColor = vec4(result, 1.0);
    }
)";
```

## Example: Time-based Effects

```cpp
// In your render loop
float time = SDL_GetTicks() / 1000.0f;

myShader->use();
myShader->setFloat("time", time);
myShader->setVec3("waveOrigin", vec3(0, 0, 0));
// ... other uniforms ...
```

```glsl
// In vertex shader
uniform float time;
uniform vec3 waveOrigin;

void main() {
    vec3 pos = aPos;
    float dist = length(pos - waveOrigin);
    pos.y += sin(dist * 2.0 - time * 4.0) * 0.5;
    // ... transform pos ...
}
```

## Shader Debugging Tips

### Check Compilation Errors

The `Shader` class automatically prints compilation errors:

```
ERROR: Shader compilation failed (VERTEX)
0:5(10): error: `unifrm` does not match any available uniform
```

### Validate Shader Before Use

```cpp
if (myShader->isValid()) {
    renderer.setShader(myShader);
} else {
    std::cerr << "Shader failed to compile!" << std::endl;
}
```

### Common Issues

1. **Uniform not found warnings**: The uniform exists in shader code but isn't used, so OpenGL optimizes it away
2. **Black screen**: Check that all required uniforms are set
3. **Incorrect colors**: Verify matrix transpose flag (default is `true` for row-major matrices)
4. **Missing geometry**: Ensure vertex attributes match layout locations

## Performance Tips

1. **Minimize shader switches**: Group draw calls by shader
2. **Cache uniform locations**: The `Shader` class does this automatically
3. **Use uniform buffers** for shared data (advanced)
4. **Avoid per-fragment expensive operations**: Move calculations to vertex shader when possible

## Advanced: Geometry Shaders

To use geometry shaders, extend the `Shader` class:

```cpp
bool compileWithGeometry(const char* vertexSrc, const char* geometrySrc, const char* fragmentSrc)
{
    // Similar to compileFromSource but include geometry shader
    GLuint geomShader = compileShader(GL_GEOMETRY_SHADER, geometrySrc);
    glAttachShader(programID, geomShader);
    // ... link and cleanup ...
}
```

## Resources

- [OpenGL Shading Language Reference](https://www.khronos.org/opengl/wiki/OpenGL_Shading_Language)
- [Learn OpenGL - Shaders](https://learnopengl.com/Getting-started/Shaders)
- [Shader Toy](https://www.shadertoy.com/) - GLSL shader examples

## Next Steps

- Try modifying the default shaders in `default_shaders.h`
- Experiment with different lighting models
- Add texture support (requires UV coordinates)
- Implement post-processing effects

