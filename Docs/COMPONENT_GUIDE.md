# Component System Guide

## Overview

The engine uses a Unity-inspired component-based architecture where GameObjects are containers for Components. This allows for flexible, modular game object behavior.

## Core Concepts

### GameObject
The fundamental entity in the scene. Every GameObject has:
- A unique name
- A TransformComponent (position, rotation, scale)
- An optional Mesh for rendering
- A collection of Components

### Component
Base class for all attachable behaviors. Components have lifecycle methods:
- `awake()` - Called when component is first created
- `start()` - Called before first update
- `update(deltaTime)` - Called every frame
- `lateUpdate(deltaTime)` - Called after all updates
- `onDestroy()` - Called when component is destroyed

### MonoBehaviour
A special Component type designed for user scripts. Provides convenient helper methods to access the GameObject's transform and common operations.

## Creating Custom Components

### Method 1: Inherit from Component

```cpp
class MyComponent : public Component
{
public:
    float myValue;
    
    void awake() override
    {
        myValue = 0.0f;
    }
    
    void update(float deltaTime) override
    {
        myValue += deltaTime;
        
        // Access GameObject
        if (gameObject)
        {
            gameObject->transform.position = vec3(myValue, 0, 0);
        }
    }
};
```

### Method 2: Inherit from MonoBehaviour (Recommended for Scripts)

```cpp
class MyScript : public MonoBehaviour
{
public:
    float speed = 5.0f;
    
    void start() override
    {
        std::cout << "MyScript starting on: " << gameObject->name << "\n";
    }
    
    void update(float deltaTime) override
    {
        // Use helper methods
        translate(vec3(speed * deltaTime, 0, 0));
        
        // Or access transform directly
        vec3 pos = position();
        rotate(vec3(0, 1.0f * deltaTime, 0));
    }
};
```

## Using Components

### Adding Components

```cpp
GameObject* obj = scene.createGameObject("MyObject");

// Add component (returns pointer for immediate configuration)
auto rotator = obj->addComponent<Rotator>();
rotator->rotationSpeed = vec3(0, 2.0f, 0);

// Add multiple components
obj->addComponent<Oscillator>();
obj->addComponent<MyCustomScript>();
```

### Getting Components

```cpp
// Get a component
Rotator* rotator = obj->getComponent<Rotator>();
if (rotator)
{
    rotator->rotationSpeed *= 2.0f;
}

// Check if component exists
if (obj->hasComponent<Rotator>())
{
    // Do something
}
```

### Removing Components

```cpp
obj->removeComponent<Rotator>();
```

## Built-in Scripts

### Rotator
Continuously rotates an object.

```cpp
auto rotator = obj->addComponent<Rotator>();
rotator->rotationSpeed = vec3(pitch, yaw, roll); // radians per second
```

### Oscillator
Makes an object oscillate along an axis.

```cpp
auto oscillator = obj->addComponent<Oscillator>();
oscillator->amplitude = 2.0f;    // How far to move
oscillator->frequency = 1.0f;    // How fast to oscillate
oscillator->axis = vec3(0, 1, 0); // Which direction
```

### Orbiter
Makes an object orbit around a point.

```cpp
auto orbiter = obj->addComponent<Orbiter>();
orbiter->center = vec3(0, 0, 0);  // Point to orbit around
orbiter->radius = 5.0f;            // Distance from center
orbiter->speed = 1.0f;             // Angular speed
```

## Advanced Component Patterns

### Component Communication

```cpp
class Shooter : public MonoBehaviour
{
public:
    void update(float deltaTime) override
    {
        // Find another component on the same GameObject
        auto rotator = gameObject->getComponent<Rotator>();
        if (rotator)
        {
            // Adjust behavior based on rotation speed
            if (rotator->rotationSpeed.magnitude() > 1.0f)
            {
                shoot();
            }
        }
    }
    
    void shoot()
    {
        // Shooting logic
    }
};
```

### Accessing Scene Objects

To access other GameObjects in the scene, you'll need to pass references or store them:

```cpp
class FollowTarget : public MonoBehaviour
{
public:
    GameObject* target = nullptr;
    float speed = 2.0f;
    
    void update(float deltaTime) override
    {
        if (target)
        {
            vec3 direction = normalize(target->transform.position - position());
            translate(direction * speed * deltaTime);
        }
    }
};

// Usage:
auto follower = obj1->addComponent<FollowTarget>();
follower->target = obj2;  // Set the target
```

### State Machine Pattern

```cpp
class Enemy : public MonoBehaviour
{
private:
    enum class State { Idle, Patrol, Chase, Attack };
    State currentState = State::Idle;
    float stateTimer = 0.0f;
    
public:
    void update(float deltaTime) override
    {
        stateTimer += deltaTime;
        
        switch (currentState)
        {
            case State::Idle:
                updateIdle(deltaTime);
                break;
            case State::Patrol:
                updatePatrol(deltaTime);
                break;
            // ... etc
        }
    }
    
private:
    void updateIdle(float deltaTime)
    {
        if (stateTimer > 2.0f)
        {
            currentState = State::Patrol;
            stateTimer = 0.0f;
        }
    }
    
    void updatePatrol(float deltaTime)
    {
        translate(transform().forward() * deltaTime);
        // ... patrol logic
    }
};
```

### Component with Configuration

```cpp
class Health : public Component
{
public:
    float maxHealth = 100.0f;
    float currentHealth;
    
    void awake() override
    {
        currentHealth = maxHealth;
    }
    
    void takeDamage(float damage)
    {
        currentHealth -= damage;
        if (currentHealth <= 0.0f)
        {
            onDeath();
        }
    }
    
    void heal(float amount)
    {
        currentHealth = std::min(currentHealth + amount, maxHealth);
    }
    
private:
    void onDeath()
    {
        // Destroy the GameObject or trigger death animation
        gameObject->setActive(false);
    }
};
```

## Best Practices

### 1. Keep Components Focused
Each component should have a single, clear responsibility.

❌ Bad:
```cpp
class PlayerController : public MonoBehaviour
{
    // Handles movement, shooting, health, inventory, UI...
};
```

✅ Good:
```cpp
class PlayerMovement : public MonoBehaviour { /* Only movement */ };
class PlayerShooting : public MonoBehaviour { /* Only shooting */ };
class PlayerHealth : public Component { /* Only health */ };
```

### 2. Use MonoBehaviour for Scripts
For gameplay scripts, inherit from MonoBehaviour to get helper methods.

```cpp
// Use this for game logic
class MyGameScript : public MonoBehaviour { };

// Use this for data-only components
class DataComponent : public Component { };
```

### 3. Initialize in awake() or start()
```cpp
void awake() override
{
    // Initialize internal state
    initialPosition = position();
}

void start() override
{
    // Get references to other components
    renderer = gameObject->getComponent<Renderer>();
}
```

### 4. Cache Frequently Accessed Data
```cpp
class Mover : public MonoBehaviour
{
private:
    vec3 startPos;  // Cache start position
    
public:
    void start() override
    {
        startPos = position();  // Cache in start()
    }
    
    void update(float deltaTime) override
    {
        // Use cached value instead of recalculating
    }
};
```

### 5. Use Public Members for Configuration
```cpp
class Projectile : public MonoBehaviour
{
public:
    float speed = 10.0f;     // Configurable
    float lifetime = 5.0f;   // Configurable
    vec3 direction = vec3::forward;
    
private:
    float timeAlive = 0.0f;  // Internal state
};

// Configure when adding
auto projectile = obj->addComponent<Projectile>();
projectile->speed = 20.0f;
projectile->lifetime = 3.0f;
```

## Component Lifecycle

```
GameObject Created
       ↓
Components Added
       ↓
awake() called (all components)
       ↓
start() called (all components)
       ↓
┌─────────────────┐
│  Game Loop      │
│  ↓              │
│  update()       │ ← Every frame
│  ↓              │
│  lateUpdate()   │ ← After all updates
└─────────────────┘
       ↓
GameObject Destroyed
       ↓
onDestroy() called
```

## Complete Example

```cpp
// Define custom component
class CameraFollow : public MonoBehaviour
{
public:
    GameObject* target = nullptr;
    vec3 offset = vec3(0, 5, -10);
    float smoothSpeed = 5.0f;
    
    void lateUpdate(float deltaTime) override
    {
        if (!target) return;
        
        vec3 desiredPos = target->transform.position + offset;
        vec3 currentPos = position();
        vec3 smoothedPos = lerp(currentPos, desiredPos, smoothSpeed * deltaTime);
        
        setPosition(smoothedPos);
        
        // Look at target
        vec3 direction = normalize(target->transform.position - smoothedPos);
        float yaw = std::atan2(direction.x(), direction.z());
        float pitch = std::asin(-direction.y());
        setRotation(vec3(pitch, yaw, 0));
    }
};

// Use in scene
int main()
{
    GameEngine engine(800, 600);
    Scene scene("Test");
    
    // Create player
    GameObject* player = scene.createGameObject("Player");
    player->setMesh(Mesh::createCube());
    auto playerMovement = player->addComponent<Rotator>();
    
    // Create camera follower
    GameObject* cameraObj = scene.createGameObject("Camera");
    auto cameraFollow = cameraObj->addComponent<CameraFollow>();
    cameraFollow->target = player;
    cameraFollow->offset = vec3(0, 3, -8);
    cameraFollow->smoothSpeed = 3.0f;
    
    engine.setActiveScene(&scene);
    engine.run(60);  // Run 60 frames
    
    return 0;
}
```

## Tips for Game Development

1. **Start Simple**: Begin with basic components and add complexity as needed
2. **Test Incrementally**: Add one component at a time and test
3. **Use Composition**: Combine simple components to create complex behaviors
4. **Profile Performance**: If a component is slow, optimize the update() method
5. **Document Public Fields**: Make it clear what each configurable value does

## Next Steps

- Create your own custom components
- Build a collection of reusable scripts
- Experiment with component combinations
- Add physics components
- Create AI behavior components
- Implement animation components
