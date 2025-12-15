# Component System Quick Reference
**Understanding Component vs MonoBehaviour**

## Quick Comparison

| Feature | Component | MonoBehaviour |
|---------|-----------|---------------|
| **Inheritance** | Base class | Extends Component |
| **Purpose** | Data storage | Game scripts |
| **Helpers** | None | position(), transform(), etc. |
| **Use For** | Health, stats, data | Player, enemies, AI |
| **Memory** | Minimal | +20 lines overhead |

## Component (Data Components)

**File**: `Core/component.h`

**Best For**:
- Health systems
- Collision data  
- Stats and attributes
- Pure data (no frequent transform access)

**Example**:
```cpp
class HealthComponent : public Component {
public:
    float health = 100.0f;
    float maxHealth = 100.0f;
    
    void takeDamage(float amount) {
        health -= amount;
        if (health <= 0) onDeath();
    }
    
    void onDeath() {
        // Direct access only
        gameObject->destroy();
    }
};
```

## MonoBehaviour (Script Components)

**File**: `Core/monoBehaviour.h`

**Best For**:
- Player controllers
- Enemy AI
- Projectile behavior
- Interactive objects
- Any gameplay script

**Helper Methods**:
```cpp
// Quick transform access
TransformComponent& transform()
vec3 position()
void setPosition(const vec3& pos)
vec3 rotation()
void setRotation(const vec3& rot)
void rotate(const vec3& eulerAngles)
void translate(const vec3& offset)
```

**Example**:
```cpp
class PlayerController : public MonoBehaviour {
    float speed = 5.0f;
    
    void update(float deltaTime) override {
        // Easy helpers
        if (Input::getKey(SDL_SCANCODE_W)) {
            translate(vec3(0, 0, -speed * deltaTime));
        }
        
        // Direct access also works
        transform().rotate(vec3(0, 90 * deltaTime, 0));
        
        // Quick position check
        if (position().y() < -10) {
            setPosition(vec3(0, 0, 0));  // Respawn
        }
    }
};
```

## Transform System

### transformComponent.h ✅ USED

**Every GameObject has a built-in transform**:
```cpp
GameObject* obj = scene.createGameObject();
obj->transform.position = vec3(0, 5, 0);
obj->transform.rotation = vec3(0, 45, 0);
obj->transform.scale = vec3(1, 1, 1);
```

**No parent hierarchies** - flat structure for now.

### ~~transform.h~~ ❌ DELETED

**Was never used** - deleted during cleanup. If you need parent hierarchies, implement in `transformComponent.h`.

## When to Use Which

### Use Component:
```cpp
// ✅ Data-heavy, no frequent transform access
class Inventory : public Component {
    std::vector<Item> items;
    // ...
};

class Stats : public Component {
    int level, experience, strength;
    // ...
};
```

### Use MonoBehaviour:
```cpp
// ✅ Behavior scripts with movement/rotation
class Enemy : public MonoBehaviour {
    void update(float dt) override {
        // Convenient helpers
        vec3 playerPos = findPlayer()->position();
        vec3 direction = playerPos - position();
        translate(normalize(direction) * speed * dt);
    }
};

class Rotator : public MonoBehaviour {
    void update(float dt) override {
        rotate(vec3(0, 90 * dt, 0));  // Easy!
    }
};
```

## Lifecycle

Both Component and MonoBehaviour have the same lifecycle:

```cpp
class MyScript : public MonoBehaviour {
    void awake() override {
        // Initialize (called immediately)
    }
    
    void start() override {
        // First frame setup
    }
    
    void update(float deltaTime) override {
        // Every frame
    }
    
    void lateUpdate(float deltaTime) override {
        // After all updates (camera follow, etc.)
    }
    
    void onDestroy() override {
        // Cleanup before deletion
    }
};
```

## Component Communication

### Get Component:
```cpp
HealthComponent* health = gameObject->getComponent<HealthComponent>();
if (health) {
    health->takeDamage(10);
}
```

### Find GameObject:
```cpp
GameObject* player = gameObject->scene->findGameObject("Player");
if (player) {
    vec3 playerPos = player->transform.position;
}
```

## Rule of Thumb

**"Will I access position/rotation often?"**
- **Yes** → Use **MonoBehaviour** (helpers save typing)
- **No** → Use **Component** (lighter weight)

**"Is this gameplay behavior or just data?"**
- **Behavior** → Use **MonoBehaviour**
- **Data** → Use **Component**
