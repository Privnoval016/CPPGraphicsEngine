//
// Component - Base class for all components that can be attached to GameObjects
// Unity architecture: Component -> Behaviour -> MonoBehaviour
//

#ifndef ENGINE_COMPONENT_H
#define ENGINE_COMPONENT_H

// Forward declarations
class GameObject;

/**
 * @class Component
 * @brief Base class for everything that can be attached to a GameObject
 * 
 * This is the root of the component hierarchy. All components (including Behaviour
 * and MonoBehaviour) inherit from this. Provides basic GameObject reference.
 */
class Component
{
public:
    GameObject* gameObject;

    Component() : gameObject(nullptr) {}
    virtual ~Component() = default;

    // No lifecycle methods here - those are in Behaviour
};

#endif //ENGINE_COMPONENT_H
