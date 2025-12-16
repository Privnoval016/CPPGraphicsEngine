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
    
    /**
     * @brief Convert screen coordinates to world space
     * @param screenPoint Screen coordinates (x, y in pixels, z as depth 0-1)
     * @param screenWidth Width of the viewport in pixels
     * @param screenHeight Height of the viewport in pixels
     * @return World space position
     * 
     * Screen coordinates: (0,0) is top-left, (width, height) is bottom-right
     * Z coordinate: 0 = near plane, 1 = far plane
     */
    vec3 screenToWorldPoint(const vec3& screenPoint, float screenWidth, float screenHeight) const
    {
        if (!camera) return vec3::zero;
        
        // Convert screen coordinates to normalized device coordinates (NDC)
        // NDC range: x,y in [-1, 1], z in [0, 1]
        float ndcX = (2.0f * screenPoint.x) / screenWidth - 1.0f;
        float ndcY = 1.0f - (2.0f * screenPoint.y) / screenHeight;  // Flip Y (screen Y is top-down)
        float ndcZ = screenPoint.z * 2.0f - 1.0f;  // Convert [0,1] to [-1,1]
        
        vec3 ndc(ndcX, ndcY, ndcZ);
        
        // Get inverse view-projection matrix
        mat4 viewProj = camera->getViewProjectionMatrix();
        mat4 invViewProj = viewProj.inverse();
        
        // Transform from clip space to world space
        // Need to use homogeneous coordinates (w component)
        float x = ndc.x;
        float y = ndc.y;
        float z = ndc.z;
        float w = 1.0f;
        
        // Manually multiply by inverse matrix
        float worldX = invViewProj.m[0][0] * x + invViewProj.m[0][1] * y + invViewProj.m[0][2] * z + invViewProj.m[0][3] * w;
        float worldY = invViewProj.m[1][0] * x + invViewProj.m[1][1] * y + invViewProj.m[1][2] * z + invViewProj.m[1][3] * w;
        float worldZ = invViewProj.m[2][0] * x + invViewProj.m[2][1] * y + invViewProj.m[2][2] * z + invViewProj.m[2][3] * w;
        float worldW = invViewProj.m[3][0] * x + invViewProj.m[3][1] * y + invViewProj.m[3][2] * z + invViewProj.m[3][3] * w;
        
        // Perspective divide
        if (std::abs(worldW) > 0.0001f) {
            worldX /= worldW;
            worldY /= worldW;
            worldZ /= worldW;
        }
        
        return vec3(worldX, worldY, worldZ);
    }

    /**
     * @brief Convert world coordinates to screen space
     * @param worldPoint World space position
     * @param screenWidth Width of the viewport in pixels
     * @param screenHeight Height of the viewport in pixels
     * @return Screen coordinates (x, y in pixels, z as depth 0-1)
     * 
     * Screen coordinates: (0,0) is top-left, (width, height) is bottom-right
     * Z coordinate: 0 = near plane, 1 = far plane
     */
    vec3 worldToScreenPoint(const vec3& worldPoint, float screenWidth, float screenHeight) const
    {
        if (!camera) return vec3::zero;
        
        // Transform to clip space
        mat4 viewProj = camera->getViewProjectionMatrix();
        
        // Manually multiply by view-projection matrix
        float x = worldPoint.x;
        float y = worldPoint.y;
        float z = worldPoint.z;
        float w = 1.0f;
        
        float clipX = viewProj.m[0][0] * x + viewProj.m[0][1] * y + viewProj.m[0][2] * z + viewProj.m[0][3] * w;
        float clipY = viewProj.m[1][0] * x + viewProj.m[1][1] * y + viewProj.m[1][2] * z + viewProj.m[1][3] * w;
        float clipZ = viewProj.m[2][0] * x + viewProj.m[2][1] * y + viewProj.m[2][2] * z + viewProj.m[2][3] * w;
        float clipW = viewProj.m[3][0] * x + viewProj.m[3][1] * y + viewProj.m[3][2] * z + viewProj.m[3][3] * w;
        
        // Perspective divide
        if (std::abs(clipW) > 0.0001f) {
            clipX /= clipW;
            clipY /= clipW;
            clipZ /= clipW;
        }
        
        // Convert from NDC [-1,1] to screen coordinates [0, width/height]
        float screenX = (clipX + 1.0f) * 0.5f * screenWidth;
        float screenY = (1.0f - clipY) * 0.5f * screenHeight;  // Flip Y (screen Y is top-down)
        float screenZ = (clipZ + 1.0f) * 0.5f;  // Convert [-1,1] to [0,1]
        
        return vec3(screenX, screenY, screenZ);
    }
};

#endif //ENGINE_CAMERA_COMPONENT_H
