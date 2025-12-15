//
// Camera Controller - WASD + Mouse look
//

#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include "../../Engine/Core/Components/monoBehaviour.h"
#include "../../Engine/Core/Components/cameraComponent.h"
#include "../../Engine/Core/Systems/input.h"
#include "../../Engine/Math/vec3.h"
#include <SDL2/SDL.h>
#include <cmath>

/**
 * @class CameraController
 * @brief First-person camera controller
 * 
 * Attach to a GameObject with a CameraComponent.
 * Controls: WASD - move, Mouse - look, Space/Shift - up/down
 */
class CameraController : public MonoBehaviour
{
public:
    float moveSpeed = 5.0f;
    float lookSpeed = 0.05f;
    float sprintMultiplier = 2.0f;

private:
    float yaw = 180.0f;  // Start at 180° to match initial rotation (looking at -Z)
    float pitch = 0.0f;
    CameraComponent* cameraComponent = nullptr;

public:
    void start() override
    {
        cameraComponent = gameObject->getComponent<CameraComponent>();
        if (!cameraComponent) {
            std::cerr << "CameraController requires CameraComponent!" << std::endl;
        }
    }

    void update(float deltaTime) override
    {
        if (!cameraComponent) return;

        Input& input = Input::getInstance();

        // Movement
        vec3 velocity = vec3::zero;

        if (input.getKey(SDL_SCANCODE_W)) velocity += forward();
        if (input.getKey(SDL_SCANCODE_S)) velocity -= forward();
        if (input.getKey(SDL_SCANCODE_A)) velocity += right();
        if (input.getKey(SDL_SCANCODE_D)) velocity -= right();
        if (input.getKey(SDL_SCANCODE_SPACE)) velocity += vec3::up;
        if (input.getKey(SDL_SCANCODE_LSHIFT)) velocity -= vec3::up;

        if (velocity.lengthSquared() > 0.01f)
        {
            velocity = velocity.normalized();
            float speed = moveSpeed;
            
            // Sprint
            if (input.getKey(SDL_SCANCODE_LCTRL)) {
                speed *= sprintMultiplier;
            }
            
            translate(velocity * speed * deltaTime);
        }

        // Look (always active for first-person feel)
        {
            // Enable relative mouse mode on first frame
            static bool initialized = false;
            if (!initialized) {
                SDL_SetRelativeMouseMode(SDL_TRUE);
                initialized = true;
            }
            
            float mouseX = input.getMouseDeltaX();
            float mouseY = input.getMouseDeltaY();

            yaw -= mouseX * lookSpeed;  // Inverted for correct left/right
            pitch += mouseY * lookSpeed;  // Inverted Y-axis

            // Clamp pitch
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;

            // Calculate forward direction
            vec3 direction;
            direction.x = std::cos(yaw * M_PI / 180.0f) * std::cos(pitch * M_PI / 180.0f);
            direction.y = std::sin(pitch * M_PI / 180.0f);
            direction.z = std::sin(yaw * M_PI / 180.0f) * std::cos(pitch * M_PI / 180.0f);

            // Update transform rotation to match look direction
            // Convert direction to euler angles
            vec3 lookDir = direction.normalized();
            float yawRad = std::atan2(lookDir.z, lookDir.x);
            float pitchRad = std::asin(lookDir.y);
            
            setRotation(vec3(pitchRad, yawRad, 0));
        }
    }
};

#endif //CAMERA_CONTROLLER_H
