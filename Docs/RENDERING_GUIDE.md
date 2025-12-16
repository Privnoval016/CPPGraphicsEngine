# Complete Rendering Guide

## Table of Contents
1. [Introduction to 3D Rendering](#introduction-to-3d-rendering)
2. [The Graphics Pipeline](#the-graphics-pipeline)
3. [Core Rendering Concepts](#core-rendering-concepts)
4. [Our Engine's Rendering Architecture](#our-engines-rendering-architecture)
5. [Step-by-Step: Rendering an Object](#step-by-step-rendering-an-object)
6. [Advanced Features](#advanced-features)
7. [Performance Optimizations](#performance-optimizations)
8. [Code Examples](#code-examples)

---

## Introduction to 3D Rendering

### What is 3D Rendering?

3D rendering is the process of converting 3D model data (vertices, triangles, colors) into a 2D image that can be displayed on your screen. Think of it like taking a photograph of a real-world scene, but instead of photographing physical objects, we're creating the entire scene mathematically.

### The Challenge

Your computer screen is a flat grid of pixels (picture elements). Each pixel can display a single color. The challenge of 3D rendering is: **How do we make a flat grid of pixels look like a three-dimensional world?**

The answer involves several mathematical transformations, clever algorithms, and specialized hardware (your GPU - Graphics Processing Unit).

### Key Terminology

Before we dive in, let's define some essential terms:

- **Vertex** (plural: vertices): A point in 3D space with coordinates (x, y, z)
- **Triangle**: The basic building block of 3D models, defined by 3 vertices
- **Mesh**: A collection of triangles that form a 3D shape
- **Texture**: An image that gets "painted" onto a 3D surface
- **Shader**: A small program that runs on the GPU to calculate colors and positions
- **Material**: Defines how a surface looks (color, shininess, textures)
- **Camera**: The viewpoint from which we render the scene
- **Transform**: Mathematical operations that move, rotate, or scale objects

---

## The Graphics Pipeline

The graphics pipeline is a sequence of stages that transform 3D data into a 2D image. Think of it like an assembly line where each stage does a specific job.

### Stage 1: Application Stage (CPU)
**What happens:** Your game/application prepares the data
- Create or load 3D models
- Set up materials and textures
- Position objects in the world
- Configure the camera and lights

**In our engine:** This happens in classes like `Scene`, `GameObject`, `MeshFilter`, and `MeshRenderer`.

### Stage 2: Vertex Processing (GPU)
**What happens:** Each vertex is transformed from 3D world space to 2D screen space

Think of this like setting up a photograph:
1. **Model Transform**: Position the object in the world (like placing a toy on a table)
2. **View Transform**: Position the camera (like holding up your camera to take the photo)
3. **Projection Transform**: Create perspective (like how the camera lens makes distant things smaller)

**In our engine:** This happens in the **vertex shader** (see `builtin_materials.h`).

### Stage 3: Rasterization (GPU)
**What happens:** Triangles are converted into pixels (fragments)

The GPU figures out which pixels on the screen are covered by each triangle. For each covered pixel, it creates a "fragment" - a potential pixel that might be drawn.

**Mathematical magic:** The GPU interpolates (smoothly blends) vertex data across the triangle's surface.

### Stage 4: Fragment Processing (GPU)
**What happens:** Each fragment gets its final color

This is where lighting, textures, and materials come together:
- Apply textures (like wrapping paper on a gift)
- Calculate lighting (how bright/dark should this point be?)
- Apply material properties (is it shiny metal or rough wood?)

**In our engine:** This happens in the **fragment shader** (see `builtin_materials.h`).

### Stage 5: Output Merging (GPU)
**What happens:** Fragments are written to the screen

- **Depth testing**: Only draw if this fragment is closer than what's already there
- **Blending**: Mix colors for transparent objects
- **Write to framebuffer**: Update the pixels that will be shown on screen

---

## Core Rendering Concepts

### 1. Vertices and Meshes

A **mesh** is like a wireframe sculpture made of triangles. Each triangle corner is a **vertex**.

#### What Data Does a Vertex Store?

```cpp
struct Vertex {
    vec3 position;    // Where is this point in 3D space?
    vec3 normal;      // Which direction is the surface facing?
    vec3 color;       // What color is this point?
    vec2 texCoords;   // Where on the texture image does this map to?
};
```

**Example:** A cube has 8 corners, but typically uses 24 vertices (4 per face) because each corner needs different normals/textures for different faces.

**In our engine:** See `Engine/Rendering/Primitives/mesh.h`
- `Mesh::createCube()` - Creates a cube mesh
- `Mesh::createSphere()` - Creates a sphere mesh
- `Mesh::createPlane()` - Creates a flat ground mesh

### 2. Transformations (Moving Things Around)

Every object has a **transformation matrix** that defines its position, rotation, and scale.

#### The Transform Hierarchy

```
World Space (the entire scene)
    ↓
Model Transform: "Where is this object?"
    ↓
View Transform: "Where is the camera looking?"
    ↓
Projection Transform: "How does the camera lens work?"
    ↓
Screen Space (your monitor pixels)
```

**Matrix multiplication** combines these transforms:
```
screenPosition = projection × view × model × vertexPosition
```

**In our engine:** See `Pose/transform.h`
- `Transform::setPosition()` - Move an object
- `Transform::setRotation()` - Rotate an object
- `Transform::setScale()` - Resize an object
- `Transform::getModelMatrix()` - Get the combined transform matrix

### 3. Textures (Surface Detail)

A **texture** is an image that gets mapped onto 3D surfaces, like wrapping paper.

#### Texture Coordinates (UVs)

Each vertex has **UV coordinates** (u, v) that say "this vertex corresponds to THIS point on the texture image."

- U = horizontal position on texture (0 = left edge, 1 = right edge)
- V = vertical position on texture (0 = bottom, 1 = top)

The GPU automatically interpolates between vertices, creating smooth texture mapping.

**In our engine:** See `Engine/Rendering/texture.h`
- `TextureLoader::loadFromFile()` - Load an image as a texture
- `Texture::bind()` - Make a texture active for rendering
- Texture filtering modes (Nearest, Linear, Bilinear, Trilinear) control how textures look when scaled

### 4. Shaders (GPU Programs)

**Shaders** are small programs written in GLSL (OpenGL Shading Language) that run on your GPU.

#### Vertex Shader
Runs once per vertex. Its job: **Calculate the screen position of each vertex**

```glsl
// Simplified vertex shader
void main() {
    // Transform vertex position to screen space
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    
    // Pass data to fragment shader
    FragPos = vec3(model * vec4(vertexPosition, 1.0));
    Normal = mat3(model) * vertexNormal;
    TexCoords = vertexTexCoords;
}
```

#### Fragment Shader
Runs once per pixel. Its job: **Calculate the final color of each pixel**

```glsl
// Simplified fragment shader
void main() {
    // Sample texture
    vec3 textureColor = texture(mainTexture, TexCoords).rgb;
    
    // Calculate lighting
    vec3 lightDir = normalize(lightPosition - FragPos);
    float brightness = max(dot(Normal, lightDir), 0.0);
    
    // Combine for final color
    vec3 finalColor = textureColor * brightness;
    FragColor = vec4(finalColor, 1.0);
}
```

**In our engine:** See `Engine/Rendering/Shaders/shader.h` and `Engine/Rendering/Materials/builtin_materials.h`

### 5. Materials (Surface Appearance)

A **material** bundles together:
- Shader (how to calculate colors)
- Textures (surface images)
- Properties (colors, shininess, metallic, etc.)

Think of materials as "recipes" for how surfaces should look.

**In our engine:** See `Engine/Rendering/Materials/material.h`
- `Material::setColor()` - Set a color property
- `Material::setFloat()` - Set a numeric property (like shininess)
- `Material::setTexture()` - Assign a texture
- `BuiltinMaterials::createStandard()` - Create a PBR material

### 6. Lighting (Making Things Look Real)

Light makes 3D objects look three-dimensional. Without lighting, everything looks flat.

#### Types of Lighting

**Ambient Light**
- Light that comes from everywhere equally
- Prevents completely black shadows
- Think: general daylight filling a room

**Diffuse Light**
- Light that reflects equally in all directions
- Surfaces facing the light are brighter
- Think: matte paint or rough surfaces

**Specular Light**
- Light that reflects in a mirror-like way
- Creates bright highlights
- Think: shiny metal or wet surfaces

#### The Phong Lighting Model

```
finalColor = ambient + diffuse + specular

ambient = ambientLight * materialColor
diffuse = lightColor * max(dot(normal, lightDir), 0) * materialColor
specular = lightColor * pow(max(dot(viewDir, reflectDir), 0), shininess)
```

**In our engine:** See `Engine/Rendering/lighting.h`
- Supports directional, point, and spot lights
- Multiple lights per scene
- Shadow mapping (casting shadows)

---

## Our Engine's Rendering Architecture

### The Command Pattern

Our renderer uses a **command-based architecture**. Instead of immediately drawing each object, we:
1. **Submit** draw commands to a queue
2. **Sort** commands for optimal rendering
3. **Flush** (execute) all commands at once

**Why?** This allows us to batch similar objects together and minimize state changes (which are slow).

```cpp
// Submit a draw command
renderer.submit(mesh, material, modelMatrix);

// Later, execute all commands
renderer.flush();
```

**Code location:** `Engine/Rendering/Core/opengl_renderer.h`

### Optimized Vertex Format

We use a **packed vertex format** that stores all vertex data efficiently:

```cpp
struct PackedVertex {
    float position[3];      // 12 bytes
    uint32_t normal;        // 4 bytes (compressed)
    uint32_t color;         // 4 bytes (RGBA packed)
    float texCoords[2];     // 8 bytes
    uint32_t tangent;       // 4 bytes (compressed)
    // Total: 32 bytes (27% smaller than unpacked!)
};
```

Normals and tangents are compressed using octahedral encoding, which saves memory and bandwidth.

**Code location:** `Engine/Rendering/Core/opengl_renderer.h` (line ~45)

### Uniform Buffer Objects (UBOs)

Instead of setting camera and light data individually for each object, we use **UBOs** - blocks of data shared across all shaders.

```cpp
struct CameraUBO {
    mat4 view;
    mat4 projection;
    vec3 position;
};

struct LightsUBO {
    Light lights[8];
    int numLights;
};
```

**Benefit:** Set once per frame instead of once per object = massive performance gain!

**Code location:** `Engine/Rendering/Core/uniform_buffer.h`

### State Caching

OpenGL state changes (like binding shaders or textures) are expensive. We cache the current state and only make changes when necessary.

```cpp
void useShader(GLuint shaderID) {
    if (currentState.boundShader != shaderID) {
        glUseProgram(shaderID);
        currentState.boundShader = shaderID;
    }
}
```

**Code location:** `Engine/Rendering/Core/opengl_renderer.h` (line ~275)

---

## Step-by-Step: Rendering an Object

Let's walk through rendering a simple cube with a texture, from start to finish.

### Step 1: Create the Mesh

```cpp
// Create a cube mesh (8 vertices, 12 triangles)
auto cubeMesh = Mesh::createCube();
```

**What happens internally:**
1. Generate 24 vertices (4 per face, each face needs different normals)
2. Each vertex gets position, normal, color, and UV coordinates
3. Define 36 indices (12 triangles × 3 vertices each)
4. Mark mesh as "dirty" so it gets uploaded to GPU

**Code:** `Engine/Rendering/Primitives/mesh.h::createCube()`

### Step 2: Load a Texture

```cpp
// Load a texture from an image file
auto brickTexture = TextureLoader::loadFromFile("Assets/Textures/brick.jpg");
```

**What happens internally:**
1. Use STB Image library to decode the image file
2. Create an OpenGL texture object
3. Upload image data to GPU memory
4. Set filtering mode (bilinear, trilinear, etc.)
5. Generate mipmaps (smaller versions for distant objects)

**Code:** `Engine/Rendering/Loaders/textureLoader.h`

### Step 3: Create a Material

```cpp
// Create a PBR material
auto brickMaterial = BuiltinMaterials::createStandard();
brickMaterial->setTexture("_MainTex", brickTexture);
brickMaterial->setColor("_Color", color(1, 1, 1));
brickMaterial->setFloat("_Metallic", 0.0f);
brickMaterial->setFloat("_Smoothness", 0.3f);
```

**What happens internally:**
1. Compile vertex and fragment shaders
2. Store texture references
3. Store material properties (colors, floats)
4. Create uniform mappings for shader communication

**Code:** `Engine/Rendering/Materials/builtin_materials.h`

### Step 4: Create a GameObject

```cpp
// Create game object with transform
auto cubeObj = scene.createGameObject("Brick Cube");
cubeObj->transform.setPosition(vec3(0, 2, 0));
cubeObj->transform.setRotation(vec3(0, 45, 0));
```

**What happens internally:**
1. Create transform with position, rotation, scale
2. Calculate model matrix from transform
3. Add to scene hierarchy

**Code:** `Engine/Core/GameObject.h` and `Engine/Core/Components/transformComponent.h`

### Step 5: Add Rendering Components

```cpp
// Add mesh and renderer components
auto meshFilter = cubeObj->addComponent<MeshFilter>();
meshFilter->setMesh(cubeMesh);

auto meshRenderer = cubeObj->addComponent<MeshRenderer>();
meshRenderer->setMaterial(brickMaterial);
```

**What happens internally:**
1. MeshFilter stores reference to mesh
2. MeshRenderer stores reference to material
3. Components register with scene's rendering system

**Code:** `Engine/Core/Components/`

### Step 6: Submit to Renderer (Every Frame)

```cpp
// In the render loop
scene.render(renderer, camera);
```

**What happens internally:**
1. Scene iterates through all GameObjects with MeshRenderer
2. For each object, get mesh, material, and transform
3. Submit draw command to renderer queue:
   ```cpp
   renderer.submit(mesh, material, transform.getModelMatrix());
   ```

**Code:** `Engine/Core/scene.h::render()`

### Step 7: Upload Mesh to GPU (First Time Only)

When a mesh is first encountered:

```cpp
// Create GPU buffers
GLuint VAO, VBO, EBO;
glGenVertexArrays(1, &VAO);
glGenBuffers(1, &VBO);
glGenBuffers(1, &EBO);

// Upload vertex data
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, vertexData.size(), vertexData.data(), GL_STATIC_DRAW);

// Upload index data
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size(), indexData.data(), GL_STATIC_DRAW);
```

**Explanation:**
- **VAO (Vertex Array Object)**: Stores the format/layout of vertex data
- **VBO (Vertex Buffer Object)**: Stores actual vertex data
- **EBO (Element Buffer Object)**: Stores triangle indices

**Code:** `Engine/Rendering/Core/opengl_renderer.h::uploadMesh()`

### Step 8: Sort and Batch Commands

```cpp
// Sort commands for optimal rendering
renderQueue.sort();
```

**Sorting criteria:**
1. Opaque before transparent
2. Group by material (minimize shader switches)
3. Group by mesh (minimize vertex buffer switches)
4. Front-to-back for opaque (early depth rejection)
5. Back-to-front for transparent (proper blending)

**Code:** `Engine/Rendering/Core/render_queue.h`

### Step 9: Execute Draw Commands

```cpp
renderer.flush();
```

**For each command:**

```cpp
// 1. Bind shader (if different from last)
if (material changed) {
    glUseProgram(material->getShader()->getID());
    material->applyToShader(); // Set uniforms and textures
}

// 2. Set per-object uniforms
shader->setMat4("model", modelMatrix);

// 3. Bind vertex array
glBindVertexArray(VAO);

// 4. Draw!
glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
```

**Code:** `Engine/Rendering/Core/opengl_renderer.h::flush()`

### Step 10: Shader Execution (GPU)

**Vertex Shader runs for each vertex:**
```glsl
void main() {
    // Transform to world space
    vec3 worldPos = (model * vec4(position, 1.0)).xyz;
    
    // Transform normal
    vec3 worldNormal = normalize(mat3(model) * normal);
    
    // Transform to screen space
    gl_Position = projection * view * vec4(worldPos, 1.0);
    
    // Pass data to fragment shader
    FragPos = worldPos;
    Normal = worldNormal;
    TexCoords = texCoords;
}
```

**Fragment Shader runs for each pixel:**
```glsl
void main() {
    // Sample texture
    vec3 albedo = texture(_MainTex, TexCoords).rgb * _Color.rgb;
    
    // Calculate lighting
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightDir);
    vec3 V = normalize(viewPos - FragPos);
    vec3 H = normalize(L + V);
    
    // Diffuse
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = albedo * NdotL;
    
    // Specular
    float NdotH = max(dot(N, H), 0.0);
    float spec = pow(NdotH, _Smoothness * 128.0);
    vec3 specular = vec3(spec) * _Metallic;
    
    // Ambient
    vec3 ambient = albedo * 0.1;
    
    // Final color
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
```

### Step 11: Display on Screen

The GPU writes the final colors to the **framebuffer**, which is displayed on your monitor.

---

## Advanced Features

### 1. Physically Based Rendering (PBR)

PBR is a realistic lighting model based on real-world physics.

**Key concepts:**
- **Albedo**: Base color (without lighting)
- **Metallic**: Is it metal (1.0) or non-metal (0.0)?
- **Roughness/Smoothness**: How rough is the surface?
- **Normal maps**: Add surface detail without more geometry

**Our Standard Material** uses PBR:
```cpp
auto pbr = BuiltinMaterials::createStandard();
pbr->setTexture("_MainTex", albedoMap);
pbr->setTexture("_MetallicGlossMap", metallicMap);
pbr->setTexture("_BumpMap", normalMap);
pbr->setFloat("_Metallic", 0.8f);
pbr->setFloat("_Smoothness", 0.9f);
```

**Code:** `Engine/Rendering/Materials/builtin_materials.h::createStandard()`

### 2. Normal Mapping

Normal maps store surface normals in a texture, allowing detailed surfaces without millions of triangles.

**How it works:**
1. Load normal map (RGB image where RGB = XYZ of normal)
2. In fragment shader, read normal from texture
3. Transform normal to world space using tangent space matrix
4. Use perturbed normal for lighting calculations

**Result:** A flat surface can look like it has bumps and grooves!

**Code:** See bump mapping in `builtin_materials.h`

### 3. Emissive Materials

Objects that emit their own light (glowing objects, neon signs, etc.).

```cpp
auto emissive = BuiltinMaterials::createStandard();
emissive->setColor("_EmissionColor", color(0, 1, 1) * 2.0f); // Bright cyan
```

**In shader:**
```glsl
vec3 emission = _EmissionColor.rgb;
FragColor = vec4(ambient + diffuse + specular + emission, 1.0);
```

### 4. Multiple Light Sources

Our engine supports up to 8 lights per scene:

```cpp
// Add directional light (sun)
auto sun = scene.addLight();
sun->type = LightType::Directional;
sun->direction = vec3(0.5f, -1.0f, -0.3f);
sun->color = vec3(1.0f, 0.95f, 0.8f);
sun->intensity = 1.0f;

// Add point light (lamp)
auto lamp = scene.addLight();
lamp->type = LightType::Point;
lamp->position = vec3(5, 3, 0);
lamp->color = vec3(1, 0.8f, 0.5f);
lamp->intensity = 5.0f;
```

**Code:** `Engine/Rendering/lighting.h`

---

## Performance Optimizations

### 1. Instanced Rendering

Render many copies of the same mesh with one draw call.

**Use case:** Forests (many trees), crowds (many people), particles

**Speedup:** 10-100x faster than individual draw calls

**Implementation:** Store per-instance data (positions, colors) in a separate buffer.

### 2. Frustum Culling

Don't render objects outside the camera's view.

**How it works:**
1. Extract camera frustum planes from view-projection matrix
2. Test each object's bounding box against frustum
3. Skip objects completely outside

**Speedup:** Saves rendering objects you can't see anyway

### 3. Occlusion Culling

Don't render objects hidden behind other objects.

**How it works:**
1. Render a low-detail version of scene to depth buffer
2. Test if objects would be visible
3. Skip fully occluded objects

### 4. Level of Detail (LOD)

Use simpler meshes for distant objects.

**Example:**
- < 10m: High detail mesh (10,000 triangles)
- 10-50m: Medium detail mesh (1,000 triangles)
- > 50m: Low detail mesh (100 triangles)

### 5. Texture Atlases

Combine many small textures into one large texture.

**Benefit:** Fewer texture switches = better batching

### 6. Our Specific Optimizations

**Packed Vertex Format** (27% memory reduction)
- Store normals as 2D octahedral coordinates
- Pack colors into 32-bit integers
- **Code:** `opengl_renderer.h` line ~45

**Uniform Buffer Objects**
- Share camera/light data across all shaders
- Set once per frame instead of per object
- **Code:** `uniform_buffer.h`

**State Caching**
- Track currently bound shader, VAO, textures
- Skip redundant state changes
- **Code:** `opengl_renderer.h::bindVAO()`, `useShader()`

**Command Queue**
- Batch and sort draw calls
- Minimize state changes
- **Code:** `render_queue.h`

**Static Mesh Caching**
- Upload mesh once, reuse forever
- Mark meshes as STATIC_DRAW
- **Code:** `opengl_renderer.h::uploadMesh()`

---

## Code Examples

### Example 1: Simple Colored Cube

```cpp
#include "GraphicsEngine.h"

int main() {
    // Initialize engine
    Engine engine(1280, 720, "My Renderer");
    Scene scene("Simple Scene");
    
    // Create camera
    auto camera = scene.createGameObject("Camera");
    auto cam = camera->addComponent<Camera>();
    camera->transform.setPosition(vec3(0, 5, 10));
    camera->transform.lookAt(vec3(0, 0, 0));
    
    // Create light
    auto light = scene.addLight();
    light->type = LightType::Directional;
    light->direction = vec3(0, -1, -0.5f);
    light->color = vec3(1, 1, 1);
    
    // Create cube
    auto cube = scene.createGameObject("Cube");
    cube->transform.setPosition(vec3(0, 0, 0));
    
    auto mesh = cube->addComponent<MeshFilter>();
    mesh->setMesh(Mesh::createCube());
    
    auto renderer = cube->addComponent<MeshRenderer>();
    auto material = BuiltinMaterials::createStandard();
    material->setColor("_Color", color(1, 0, 0)); // Red
    renderer->setMaterial(material);
    
    // Game loop
    while (engine.isRunning()) {
        engine.update();
        scene.update();
        scene.render(engine.getRenderer(), cam);
        engine.present();
    }
    
    return 0;
}
```

### Example 2: Textured Object with Normal Map

```cpp
scene.onOpenGLReady([&](Scene& s) {
    // Load textures
    auto albedo = TextureLoader::loadFromFile("Assets/Textures/brick_albedo.jpg");
    auto normal = TextureLoader::loadFromFile("Assets/Textures/brick_normal.jpg");
    auto metallic = TextureLoader::loadFromFile("Assets/Textures/brick_metallic.jpg");
    
    // Create PBR material
    auto brickMat = BuiltinMaterials::createStandard();
    brickMat->setTexture("_MainTex", albedo);
    brickMat->setTexture("_BumpMap", normal);
    brickMat->setTexture("_MetallicGlossMap", metallic);
    brickMat->setFloat("_Metallic", 0.1f);
    brickMat->setFloat("_Smoothness", 0.4f);
    brickMat->setFloat("_BumpScale", 1.0f);
    
    // Create wall
    auto wall = s.createGameObject("Brick Wall");
    wall->transform.setScale(vec3(10, 10, 1));
    wall->addComponent<MeshFilter>()->setMesh(Mesh::createPlane());
    wall->addComponent<MeshRenderer>()->setMaterial(brickMat);
});
```

### Example 3: Multiple Objects with Batching

```cpp
// Create many cubes - engine will batch them efficiently
for (int x = -10; x <= 10; x += 2) {
    for (int z = -10; z <= 10; z += 2) {
        auto cube = scene.createGameObject("Cube_" + std::to_string(x) + "_" + std::to_string(z));
        cube->transform.setPosition(vec3(x, 0, z));
        
        // All use same mesh and material = efficient batching
        cube->addComponent<MeshFilter>()->setMesh(sharedCubeMesh);
        cube->addComponent<MeshRenderer>()->setMaterial(sharedMaterial);
    }
}
// Result: 121 cubes rendered very efficiently!
```

### Example 4: Custom Material

```cpp
// Define custom shader
std::string vertexShader = R"(
    #version 410 core
    layout(location = 0) in vec3 position;
    layout(location = 1) in vec3 normal;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    out vec3 FragNormal;
    
    void main() {
        gl_Position = projection * view * model * vec4(position, 1.0);
        FragNormal = mat3(model) * normal;
    }
)";

std::string fragmentShader = R"(
    #version 410 core
    in vec3 FragNormal;
    out vec4 FragColor;
    
    uniform vec3 _Color;
    uniform float _Time;
    
    void main() {
        // Animated rainbow effect
        vec3 rainbow = vec3(
            sin(_Time) * 0.5 + 0.5,
            sin(_Time + 2.0) * 0.5 + 0.5,
            sin(_Time + 4.0) * 0.5 + 0.5
        );
        
        float brightness = max(dot(normalize(FragNormal), vec3(0, 1, 0)), 0.0);
        FragColor = vec4(rainbow * _Color * brightness, 1.0);
    }
)";

// Create material
auto customMat = std::make_shared<Material>();
auto shader = std::make_shared<Shader>();
shader->compileFromSource(vertexShader, fragmentShader);
customMat->setShader(shader);
customMat->setColor("_Color", color(1, 1, 1));

// Use material
renderer->setMaterial(customMat);

// Update time uniform each frame
customMat->setFloat("_Time", engine.getTime());
```

### Example 5: Loading 3D Models

```cpp
// Load a 3D model from OBJ file
auto mesh = ModelLoader::loadFromFile("Assets/Models/character.obj");

if (mesh) {
    auto character = scene.createGameObject("Character");
    character->addComponent<MeshFilter>()->setMesh(mesh);
    
    // Load and apply texture
    auto texture = TextureLoader::loadFromFile("Assets/Textures/character_diffuse.png");
    auto material = BuiltinMaterials::createStandard();
    material->setTexture("_MainTex", texture);
    
    character->addComponent<MeshRenderer>()->setMaterial(material);
}
```

---

## Troubleshooting Common Issues

### Black Screen
- **Check:** Is the camera positioned correctly?
- **Check:** Are there lights in the scene?
- **Check:** Is the object within the camera's view frustum?

### Object Not Appearing
- **Check:** Does the GameObject have both MeshFilter and MeshRenderer?
- **Check:** Is the mesh valid (non-zero triangles)?
- **Check:** Is the material assigned and valid?

### Texture Not Showing
- **Check:** Does the mesh have UV coordinates?
- **Check:** Is the texture loaded successfully?
- **Check:** Did you call `material->setTexture()`?
- **Check:** Does the shader support textures?

### Low Frame Rate
- **Check:** How many triangles are you rendering?
- **Check:** Are you creating new meshes/materials every frame? (Don't!)
- **Check:** Are you calling `glGetError()` every frame? (Very slow!)
- **Use:** Profiler to identify bottlenecks

### Weird Lighting Artifacts
- **Check:** Are normals calculated correctly?
- **Check:** Is the normal transformation using `mat3(model)` or `transpose(inverse(model))`?
- **Check:** Are normals normalized in the shader?

---

## Summary

Rendering 3D graphics is a complex pipeline involving:

1. **Data preparation** (meshes, materials, transforms)
2. **GPU upload** (vertex buffers, textures)
3. **Transformation** (model → world → view → screen)
4. **Rasterization** (triangles → fragments)
5. **Shading** (calculate colors with lighting and textures)
6. **Output** (write to screen)

Our engine optimizes this with:
- **Packed vertex format** (27% memory savings)
- **Command queue** (batching and sorting)
- **UBOs** (shared data across shaders)
- **State caching** (minimize redundant GPU calls)
- **PBR materials** (realistic lighting)

To render an object, you need:
1. A **mesh** (geometry)
2. A **material** (appearance)
3. A **transform** (position/rotation/scale)
4. A **camera** (viewpoint)
5. **Lights** (illumination)

The rendering loop:
```
For each frame:
    1. Update scene (physics, logic)
    2. Submit draw commands
    3. Sort and batch commands
    4. Execute commands (GPU rendering)
    5. Present to screen
```

Happy rendering! 🎨

---

## Further Reading

- **OpenGL Tutorial**: learnopengl.com
- **Graphics Programming**: realtimerendering.com
- **Shader Programming**: thebookofshaders.com
- **PBR Theory**: learnopengl.com/PBR/Theory
- **Our Engine Docs**:
  - `MATERIAL_SYSTEM.md` - Material and shader details
  - `ARCHITECTURE.md` - Overall engine architecture
  - `GETTING_STARTED.md` - Quick start guide
