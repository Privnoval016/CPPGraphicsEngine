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
    friend class GameObject;  // Allow GameObject to call protected lifecycle methods

public:
    bool enabled;

    Behaviour() : Component(), enabled(true) {}
    virtual ~Behaviour() = default;

    // Enable/disable control
    void setEnabled(bool value) { enabled = value; }
    bool isEnabled() const { return enabled; }

protected:
    // Lifecycle methods - override in derived classes
    // Protected to prevent external calls, only GameObject can invoke these
    virtual void awake() {}
    virtual void start() {}
    virtual void update(float deltaTime) {}
    virtual void lateUpdate(float deltaTime) {}
    virtual void onDestroy() {}
};

#endif //ENGINE_BEHAVIOUR_H
