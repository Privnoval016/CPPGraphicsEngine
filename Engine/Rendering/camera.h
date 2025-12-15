//
// Created by Graphics Engine
//

#ifndef CAMERA_H
#define CAMERA_H

#include "../Math/vec3.h"
#include "../Math/mat4.h"
#include <cmath>

class Camera
{
public:
    vec3 position;
    vec3 rotation; // Euler angles (pitch, yaw, roll)
    
    float fieldOfView; // In radians
    float aspectRatio;
    float nearPlane;
    float farPlane;

    // Camera vectors
    vec3 forward;
    vec3 right;
    vec3 up;

    Camera()
        : position(0, 0, -5),
          rotation(0, 0, 0),
          fieldOfView(60.0f * 3.14159f / 180.0f),
          aspectRatio(16.0f / 9.0f),
          nearPlane(0.1f),
          farPlane(1000.0f),
          forward(0, 0, 1),
          right(1, 0, 0),
          up(0, 1, 0)
    {
        updateVectors();
    }

    Camera(const vec3& pos, const vec3& target)
        : position(pos),
          rotation(0, 0, 0),
          fieldOfView(60.0f * 3.14159f / 180.0f),
          aspectRatio(16.0f / 9.0f),
          nearPlane(0.1f),
          farPlane(1000.0f),
          forward(0, 0, 1),
          right(1, 0, 0),
          up(0, 1, 0)
    {
        lookAt(target);
    }

    Camera(const vec3& pos, float fov, float aspect, float near, float far)
        : position(pos),
          rotation(0, 0, 0),
          fieldOfView(fov),
          aspectRatio(aspect),
          nearPlane(near),
          farPlane(far),
          forward(0, 0, 1),
          right(1, 0, 0),
          up(0, 1, 0)
    {
        updateVectors();
    }

    // Get view matrix (world to camera space)
    mat4 getViewMatrix() const
    {
        vec3 target = position + forward;
        return mat4::lookAt(position, target, up);
    }

    // Get projection matrix (camera to clip space)
    mat4 getProjectionMatrix() const
    {
        return mat4::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
    }

    // Get combined view-projection matrix
    mat4 getViewProjectionMatrix() const
    {
        return getProjectionMatrix() * getViewMatrix();
    }

    // Rotate camera
    void rotate(float pitch, float yaw, float roll = 0.0f)
    {
        rotation = vec3(rotation.x + pitch, rotation.y + yaw, rotation.z + roll);
        
        // Clamp pitch to prevent gimbal lock
        const float maxPitch = 89.0f * 3.14159f / 180.0f;
        if (rotation.x > maxPitch) rotation[0] = maxPitch;
        if (rotation.x < -maxPitch) rotation[0] = -maxPitch;
        
        updateVectors();
    }

    // Move camera in local space
    void translate(const vec3& offset)
    {
        position += right * offset.x;
        position += up * offset.y;
        position += forward * offset.z;
    }

    // Move camera in world space
    void translateWorld(const vec3& offset)
    {
        position += offset;
    }

    // Look at a target
    void lookAt(const vec3& target)
    {
        vec3 direction = target - position.normalized();
        
        // Calculate yaw and pitch from direction
        rotation[1] = std::atan2(direction.x, direction.z); // yaw
        rotation[0] = std::asin(-direction.y); // pitch
        rotation[2] = 0.0f; // roll
        
        updateVectors();
    }

    // Set aspect ratio (useful when window resizes)
    void setAspectRatio(float aspect)
    {
        aspectRatio = aspect;
    }

    void setPerspective(float fov, float aspect, float near, float far)
    {
        fieldOfView = fov;
        aspectRatio = aspect;
        nearPlane = near;
        farPlane = far;
    }

    // Getters for direction vectors
    vec3 getForward() const { return forward; }
    vec3 getRight() const { return right; }
    vec3 getUp() const { return up; }

    // Setter for forward direction
    void setForward(const vec3& newForward)
    {
        forward = newForward.normalized();
        right = vec3::cross(forward, vec3(0, 1, 0)).normalized();
        up = vec3::cross(right, forward).normalized();
    }

private:
    void updateVectors()
    {
        // Calculate forward vector from rotation
        vec3 f;
        f[0] = std::sin(rotation.y) * std::cos(rotation.x);
        f[1] = -std::sin(rotation.x);
        f[2] = std::cos(rotation.y) * std::cos(rotation.x);
        forward = f.normalized();

        // Calculate right vector
        right = vec3::cross(forward, vec3(0, 1, 0)).normalized();

        // Calculate up vector
        up = vec3::cross(right, forward).normalized();
    }
};

#endif //CAMERA_H
