//
// Behaviour - Components with enable/disable and lifecycle methods
// Unity architecture: Component -> Behaviour -> MonoBehaviour
//

#ifndef ENGINE_BEHAVIOUR_H
#define ENGINE_BEHAVIOUR_H

#include "component.h"

/**
 * @class Behaviour
 * @brief Component with lifecycle methods and enabled state
 * 
 * Extends Component to add:
 * - Enable/disable functionality
 * - Lifecycle methods (awake, start, update, lateUpdate, onDestroy)
 * 
 * This is the base for MonoBehaviour. Use Behaviour for engine-internal
 * components that need lifecycle methods but aren't user scripts.
 */
class Behaviour : public Component
{
public:
    bool enabled;

    Behaviour() : Component(), enabled(true) {}
    virtual ~Behaviour() = default;

    // Lifecycle methods - override in derived classes
    virtual void awake() {}
    virtual void start() {}
    virtual void update(float deltaTime) {}
    virtual void lateUpdate(float deltaTime) {}
    virtual void onDestroy() {}

    // Enable/disable control
    void setEnabled(bool value) { enabled = value; }
    bool isEnabled() const { return enabled; }
};

#endif //ENGINE_BEHAVIOUR_H
