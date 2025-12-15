# Quick Reference Guide

## Math Operations

### vec3
```cpp
vec3 v1(1, 2, 3);
vec3 v2(4, 5, 6);

// Operations
vec3 sum = v1 + v2;
vec3 diff = v1 - v2;
vec3 scaled = v1 * 2.0f;
float d = dot(v1, v2);
vec3 c = cross(v1, v2);
vec3 n = normalize(v1);
float len = v1.magnitude();

// Constants
vec3::zero      // (0, 0, 0)
vec3::one       // (1, 1, 1)
vec3::up        // (0, 1, 0)
vec3::forward   // (0, 0, 1)
vec3::right     // (1, 0, 0)
```

### mat4
```cpp
// Creation
mat4 identity = mat4::identity();
mat4 trans = mat4::translation(vec3(1, 2, 3));
mat4 scale = mat4::scale(vec3(2, 2, 2));
mat4 rot = mat4::rotationY(3.14159f / 4.0f);

// Transformations
vec3 point = mat.transformPoint(vec3(1, 2, 3));
vec3 dir = mat.transformDirection(vec3(0, 1, 0));

// Camera matrices
mat4 view = mat4::lookAt(eye, center, up);
mat4 proj = mat4::perspective(fov, aspect, near, far);
```

## GameObject Operations

```cpp
// Create
GameObject* obj = scene.createGameObject("Name");

// Transform
obj->transform.position = vec3(1, 2, 3);
obj->transform.rotation = vec3(0, 3.14f, 0);
obj->transform.scale = vec3(1, 1, 1);
obj->transform.translate(vec3(0.1f, 0, 0));
obj->transform.rotate(vec3(0, 0.01f, 0));

// Mesh
obj->setMesh(Mesh::createCube());
obj->setMesh(Mesh::createSphere());
obj->setMesh(Mesh::createPlane());

// Components
auto comp = obj->addComponent<MyComponent>();
MyComponent* c = obj->getComponent<MyComponent>();
bool has = obj->hasComponent<MyComponent>();
obj->removeComponent<MyComponent>();

// State
obj->setActive(true);
bool active = obj->isActive();
```

## Scene Setup

```cpp
Scene scene("MyScene");

// Camera
scene.mainCamera.position = vec3(0, 5, -10);
scene.mainCamera.lookAt(vec3(0, 0, 0));
scene.mainCamera.fieldOfView = 60.0f * 3.14159f / 180.0f;
scene.mainCamera.nearPlane = 0.1f;
scene.mainCamera.farPlane = 1000.0f;

// Lights
scene.addLight(Light::directional(vec3(0, -1, 0)));
scene.addLight(Light::point(vec3(5, 5, 5), color(1, 1, 1), 1.0f, 20.0f));

// Background
scene.backgroundColor = color(0.2f, 0.2f, 0.3f);
```

## Engine Setup

```cpp
GameEngine engine(800, 600);

// Scene
engine.setActiveScene(&scene);

// Rendering
engine.rasterizer.renderMode = Rasterizer::RenderMode::Solid;
engine.rasterizer.backfaceCulling = true;
engine.rasterizer.wireframeColor = color(1, 1, 1);

// Run
engine.run(numFrames, deltaTime);
engine.runInteractive();  // Continuous loop

// Output
engine.saveFrame("output.ppm");
engine.outputFrame();  // To stdout
```

## Component Template

```cpp
class MyComponent : public MonoBehaviour
{
public:
    // Public configuration
    float mySpeed = 1.0f;
    vec3 myDirection = vec3::forward;
    
private:
    // Private state
    float timer = 0.0f;
    
public:
    void awake() override
    {
        // Initialize
    }
    
    void start() override
    {
        // Setup after all objects created
    }
    
    void update(float deltaTime) override
    {
        // Frame logic
        timer += deltaTime;
        translate(myDirection * mySpeed * deltaTime);
    }
    
    void lateUpdate(float deltaTime) override
    {
        // After all updates
    }
    
    void onDestroy() override
    {
        // Cleanup
    }
};
```

## Mesh Creation

```cpp
// Procedural meshes
auto cube = Mesh::createCube(1.0f);
auto sphere = Mesh::createSphere(1.0f, subdivisions);
auto plane = Mesh::createPlane(10.0f, 10.0f);

// Custom mesh
auto mesh = std::make_shared<Mesh>();
mesh->vertices.push_back(Vertex(vec3(0, 0, 0), vec3(0, 1, 0)));
mesh->triangles.push_back(Triangle(0, 1, 2));
mesh->calculateNormals();
```

## Common Patterns

### Rotating Object
```cpp
auto rotator = obj->addComponent<Rotator>();
rotator->rotationSpeed = vec3(0, 2.0f, 0);
```

### Following Target
```cpp
class Follower : public MonoBehaviour
{
public:
    GameObject* target;
    float speed = 2.0f;
    
    void update(float dt) override
    {
        if (target)
        {
            vec3 dir = normalize(target->transform.position - position());
            translate(dir * speed * dt);
        }
    }
};
```

### Timer-Based Action
```cpp
class TimedAction : public MonoBehaviour
{
    float timer = 0.0f;
    float interval = 2.0f;
    
    void update(float dt) override
    {
        timer += dt;
        if (timer >= interval)
        {
            doAction();
            timer = 0.0f;
        }
    }
};
```

### Spawner
```cpp
class Spawner : public MonoBehaviour
{
public:
    float spawnRate = 1.0f;
    
private:
    float timer = 0.0f;
    Scene* scene;  // Store scene reference
    
public:
    void update(float dt) override
    {
        timer += dt;
        if (timer >= spawnRate)
        {
            spawn();
            timer = 0.0f;
        }
    }
    
    void spawn()
    {
        // Create new object in scene
        // (Need scene reference passed in)
    }
};
```

## Lighting

```cpp
// Directional (sun)
Light sun = Light::directional(
    vec3(0.5f, -1.0f, 0.3f),  // Direction
    color(1, 1, 0.9f),         // Color
    1.0f                       // Intensity
);

// Point light
Light lamp = Light::point(
    vec3(5, 3, 0),      // Position
    color(1, 0.8f, 0.6f), // Color
    1.5f,               // Intensity
    20.0f               // Range
);

scene.addLight(sun);
scene.addLight(lamp);
```

## Color Operations

```cpp
color c1(1, 0, 0);        // Red
color c2(0, 1, 0);        // Green
color result = c1 * 0.5f;  // Dim
color mixed = c1 + c2;     // Mix
```

## Transform Helpers

```cpp
// Get local axes
vec3 fwd = obj->transform.forward();
vec3 right = obj->transform.right();
vec3 up = obj->transform.up();

// Move in local space
obj->transform.translate(obj->transform.forward() * speed);

// Get model matrix for rendering
mat4 model = obj->transform.getModelMatrix();
```

## File I/O

```cpp
// Save single frame
engine.saveFrame("frame.ppm");

// Generate sequence
for (int i = 0; i < 100; i++)
{
    engine.run(1);
    engine.saveFrame("frame_" + std::to_string(i) + ".ppm");
}

// Pipe to stdout
engine.outputFrame();  // Can redirect: ./app > image.ppm
```

## Render Modes

```cpp
// Wireframe only
engine.rasterizer.renderMode = Rasterizer::RenderMode::Wireframe;

// Solid shading (default)
engine.rasterizer.renderMode = Rasterizer::RenderMode::Solid;

// Both
engine.rasterizer.renderMode = Rasterizer::RenderMode::SolidWireframe;

// Culling
engine.rasterizer.backfaceCulling = true;  // Skip back faces
engine.rasterizer.backfaceCulling = false; // Show both sides
```

## Debugging

```cpp
// Print object info
std::cout << "Object: " << obj->name << "\n";
std::cout << "Position: " << obj->transform.position << "\n";
std::cout << "Active: " << obj->isActive() << "\n";

// Print scene info
auto objects = scene.getAllGameObjects();
std::cout << "GameObject count: " << objects.size() << "\n";

// Find object
GameObject* obj = scene.findGameObject("MyObject");
if (obj) { /* found */ }
```

## Performance Tips

```cpp
// Cache frequently accessed data
class Mover : public MonoBehaviour
{
private:
    vec3 cachedForward;
    
public:
    void start() override
    {
        cachedForward = transform().forward();
    }
};

// Avoid expensive operations in update
void update(float dt) override
{
    // ❌ Bad: recalculates every frame
    vec3 dir = normalize(target->transform.position - position());
    
    // ✅ Better: only when needed
    if (shouldRecalculate)
    {
        direction = normalize(target->transform.position - position());
        shouldRecalculate = false;
    }
}
```

## Constants

```cpp
const float PI = 3.14159265359f;
const float DEG_TO_RAD = PI / 180.0f;
const float RAD_TO_DEG = 180.0f / PI;

// Usage
float angleRad = 45.0f * DEG_TO_RAD;
float angleDeg = angleRad * RAD_TO_DEG;
```
