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

    // ===== Convenience Helper Methods =====
    // These make script writing easier by reducing boilerplate

    /**
     * @brief Quick access to transform component
     */
    TransformComponent& transform()
    {
        return gameObject->transform;
    }

    /**
     * @brief Get current position
     */
    vec3 position() const
    {
        return gameObject->transform.getWorldPosition();
    }

    /**
     * @brief Set position directly
     */
    void setPosition(const vec3& pos)
    {
        gameObject->transform.setWorldPosition(pos);
    }

    /**
     * @brief Get current rotation (Euler angles)
     */
    vec3 rotation() const
    {
        return gameObject->transform.getWorldRotation();
    }

    /**
     * @brief Set rotation directly
     */
    void setRotation(const vec3& rot)
    {
        gameObject->transform.setWorldRotation(rot);
    }

    /**
     * @brief Rotate by Euler angles
     */
    void rotate(const vec3& eulerAngles)
    {
        gameObject->transform.rotate(eulerAngles);
    }

    /**
     * @brief Move by offset
     */
    void translate(const vec3& offset)
    {
        gameObject->transform.translate(offset);
    }

    /**
     * @brief Get forward direction
     */
    vec3 forward() const
    {
        return gameObject->transform.forward();
    }

    /**
     * @brief Get right direction
     */
    vec3 right() const
    {
        return gameObject->transform.right();
    }

    /**
     * @brief Get up direction
     */
    vec3 up() const
    {
        return gameObject->transform.up();
    }
};

#endif //ENGINE_MONOBEHAVIOUR_H
