//
// Camera Component - Handles camera view and projection
//

#ifndef ENGINE_CAMERA_COMPONENT_H
#define ENGINE_CAMERA_COMPONENT_H

#include "behaviour.h"
#include "../../Math/vec3.h"
#include "../../Math/mat4.h"
#include "../../Rendering/camera.h"
#include <memory>

/**
 * @class CameraComponent
 * @brief Camera component that can be attached to GameObjects
 * 
 * Provides camera functionality as a component. The GameObject's
 * transform controls the camera position and orientation.
 */
class CameraComponent : public Behaviour
{
private:
    std::shared_ptr<Camera> camera;
    float fieldOfView;  // In radians
    float aspectRatio;
    float nearPlane;
    float farPlane;
    bool isPerspective;

public:
    CameraComponent()
        : Behaviour(),
          camera(std::make_shared<Camera>()),
          fieldOfView(70.0f * 3.14159f / 180.0f),
          aspectRatio(16.0f / 9.0f),
          nearPlane(0.1f),
          farPlane(100.0f),
          isPerspective(true)
    {
    }

    void awake() override
    {
        updateCamera();
    }

    void lateUpdate(float deltaTime) override
    {
        // Update camera after all transforms have been updated
        updateCamera();
    }

    void updateCamera()
    {
        if (!gameObject) return;

        // Update camera position and orientation from transform
        vec3 worldPos = gameObject->transform.getWorldPosition();
        camera->position = worldPos;
        camera->setForward(gameObject->transform.forward());

        // Update projection
        if (isPerspective) {
            camera->setPerspective(fieldOfView, aspectRatio, nearPlane, farPlane);
        } else {
            // Orthographic - for now just use perspective
            // TODO: Add setOrthographic to Camera class
            camera->setPerspective(fieldOfView, aspectRatio, nearPlane, farPlane);
        }
    }

    // Getters
    Camera* getCamera() { return camera.get(); }
    const Camera* getCamera() const { return camera.get(); }

    float getFieldOfView() const { return fieldOfView; }
    float getAspectRatio() const { return aspectRatio; }
    float getNearPlane() const { return nearPlane; }
    float getFarPlane() const { return farPlane; }
    bool getIsPerspective() const { return isPerspective; }

    // Setters
    void setFieldOfView(float fov) 
    { 
        fieldOfView = fov;
        updateCamera();
    }

    void setAspectRatio(float ratio) 
    { 
        aspectRatio = ratio;
        updateCamera();
    }

    void setNearPlane(float near) 
    { 
        nearPlane = near;
        updateCamera();
    }

    void setFarPlane(float far) 
    { 
        farPlane = far;
        updateCamera();
    }

    void setPerspective(float fov, float aspect, float near, float far)
    {
        isPerspective = true;
        fieldOfView = fov;
        aspectRatio = aspect;
        nearPlane = near;
        farPlane = far;
        updateCamera();
    }

    void setOrthographic(float near, float far)
    {
        isPerspective = false;
        nearPlane = near;
        farPlane = far;
        updateCamera();
    }

    // Convenience methods
    vec3 screenToWorldPoint(const vec3& screenPoint) const
    {
        // TODO: Implement screen to world conversion
        return screenPoint;
    }

    vec3 worldToScreenPoint(const vec3& worldPoint) const
    {
        // TODO: Implement world to screen conversion
        return worldPoint;
    }
};

#endif //ENGINE_CAMERA_COMPONENT_H
