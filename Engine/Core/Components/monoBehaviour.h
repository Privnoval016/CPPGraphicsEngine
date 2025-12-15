//
// MonoBehaviour - Base class for user-written scripts
// Unity architecture: Component -> Behaviour -> MonoBehaviour
//

#ifndef ENGINE_MONOBEHAVIOUR_H
#define ENGINE_MONOBEHAVIOUR_H

#include "behaviour.h"
#include "../gameObject.h"
#include "transformComponent.h"
#include "../../Math/vec3.h"

/**
 * @class MonoBehaviour
 * @brief The base class for all user-written scripts
 * 
 * MonoBehaviour extends Behaviour to add convenience methods for
 * common GameObject operations. This is what users inherit from
 * when writing gameplay scripts.
 * 
 * Example:
 * @code
 * class PlayerController : public MonoBehaviour {
 *     void update(float deltaTime) override {
 *         if (Input::getKey(SDL_SCANCODE_W)) {
 *             translate(vec3(0, 0, -speed * deltaTime));
 *         }
 *     }
 * };
 * @endcode
 */
class MonoBehaviour : public Behaviour
{
public:
    MonoBehaviour() : Behaviour() {}
    virtual ~MonoBehaviour() = default;

    // Lifecycle methods - override these in your scripts
    void awake() override {}
    void start() override {}
    void update(float deltaTime) override {}
    void lateUpdate(float deltaTime) override {}
    void onDestroy() override {}

    // ===== Transform Access =====
    
    /**
     * @brief Quick access to transform component
     */
    TransformComponent& transform()
    {
        return gameObject->transform;
    }
};

#endif //ENGINE_MONOBEHAVIOUR_H
