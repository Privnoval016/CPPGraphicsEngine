//
// TransformComponent - Position, rotation, scale with parent-child hierarchy
//

#ifndef ENGINE_TRANSFORM_COMPONENT_H
#define ENGINE_TRANSFORM_COMPONENT_H

#include "component.h"
#include "../../Math/vec3.h"
#include "../../Math/mat4.h"
#include <vector>
#include <algorithm>

/**
 * @class TransformComponent
 * @brief Manages GameObject spatial properties with hierarchical transforms
 * 
 * Every GameObject has a TransformComponent that defines its position,
 * rotation, and scale. Supports parent-child relationships for hierarchical
 * transformations (e.g., attaching weapon to player hand).
 */
class TransformComponent
{
private:
    TransformComponent* parentTransform;
    std::vector<TransformComponent*> children;
    bool isDirty;

    // Local transform (relative to parent)
    vec3 localPosition;
    vec3 localRotation;  // Euler angles in radians
    vec3 localScale;

    // Cached world transform
    mutable vec3 cachedWorldPosition;
    mutable vec3 cachedWorldRotation;
    mutable vec3 cachedWorldScale;
    mutable mat4 cachedWorldMatrix;
    mutable bool worldCacheDirty;

    void markDirty()
    {
        isDirty = true;
        worldCacheDirty = true;
        // Propagate dirty flag to all children
        for (auto* child : children) {
            if (!child->isDirty) {  // Only propagate if child isn't already dirty
                child->markDirty();
            }
        }
    }

    void updateWorldCache() const
    {
        if (!worldCacheDirty) return;

        if (parentTransform) {
            // Get parent's world matrix
            mat4 parentWorld = parentTransform->getWorldMatrix();
            
            // Calculate world matrix
            mat4 localMat = mat4::translation(localPosition) * 
                          mat4::euler(localRotation) * 
                          mat4::scale(localScale);
            cachedWorldMatrix = parentWorld * localMat;
            
            // Extract world position, rotation, scale from matrix
            // For simplicity, we'll just use the local values scaled by parent
            // (Full decomposition is complex and rarely needed)
            cachedWorldPosition = parentTransform->getWorldPosition() + localPosition;
            cachedWorldRotation = parentTransform->getWorldRotation() + localRotation;
            cachedWorldScale = vec3(
                parentTransform->getWorldScale().x() * localScale.x(),
                parentTransform->getWorldScale().y() * localScale.y(),
                parentTransform->getWorldScale().z() * localScale.z()
            );
        } else {
            // No parent - world = local
            cachedWorldPosition = localPosition;
            cachedWorldRotation = localRotation;
            cachedWorldScale = localScale;
            cachedWorldMatrix = mat4::translation(localPosition) * 
                              mat4::euler(localRotation) * 
                              mat4::scale(localScale);
        }

        worldCacheDirty = false;
    }

public:
    // Note: Use setter methods (setPosition, setRotation, setScale) 
    // instead of direct member access for proper hierarchy support

    TransformComponent()
        : parentTransform(nullptr),
          isDirty(false),
          worldCacheDirty(true),
          localPosition(vec3::zero),
          localRotation(vec3::zero),
          localScale(vec3::one)
    {
    }

    TransformComponent(const vec3& pos, const vec3& rot, const vec3& scl)
        : parentTransform(nullptr),
          isDirty(false),
          worldCacheDirty(true),
          localPosition(pos),
          localRotation(rot),
          localScale(scl)
    {
    }

    ~TransformComponent()
    {
        // Detach from parent
        setParent(nullptr);
        
        // Detach all children
        while (!children.empty()) {
            children.back()->setParent(nullptr);
        }
    }

    // ===== Parent-Child Hierarchy =====

    /**
     * @brief Set the parent transform (nullptr for world root)
     */
    void setParent(TransformComponent* newParent)
    {
        if (parentTransform == newParent) return;

        // Remove from old parent's children list
        if (parentTransform) {
            auto& siblings = parentTransform->children;
            siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
        }

        parentTransform = newParent;

        // Add to new parent's children list
        if (parentTransform) {
            parentTransform->children.push_back(this);
        }

        markDirty();
    }

    /**
     * @brief Get parent transform (nullptr if root)
     */
    TransformComponent* getParent() const
    {
        return parentTransform;
    }

    /**
     * @brief Get child at index
     */
    TransformComponent* getChild(size_t index) const
    {
        return index < children.size() ? children[index] : nullptr;
    }

    /**
     * @brief Get number of children
     */
    size_t getChildCount() const
    {
        return children.size();
    }

    // ===== Local Space (relative to parent) =====

    vec3 getLocalPosition() const { return localPosition; }
    vec3 getLocalRotation() const { return localRotation; }
    vec3 getLocalScale() const { return localScale; }

    void setLocalPosition(const vec3& pos)
    {
        localPosition = pos;
        markDirty();
    }

    void setLocalRotation(const vec3& rot)
    {
        localRotation = rot;
        markDirty();
    }

    void setLocalScale(const vec3& scl)
    {
        localScale = scl;
        markDirty();
    }

    // ===== World Space =====

    vec3 getWorldPosition() const
    {
        updateWorldCache();
        return cachedWorldPosition;
    }

    vec3 getWorldRotation() const
    {
        updateWorldCache();
        return cachedWorldRotation;
    }

    vec3 getWorldScale() const
    {
        updateWorldCache();
        return cachedWorldScale;
    }

    mat4 getWorldMatrix() const
    {
        updateWorldCache();
        return cachedWorldMatrix;
    }

    void setWorldPosition(const vec3& pos)
    {
        if (parentTransform) {
            // Convert world to local
            vec3 parentPos = parentTransform->getWorldPosition();
            localPosition = pos - parentPos;
        } else {
            localPosition = pos;
        }
        markDirty();
    }

    void setWorldRotation(const vec3& rot)
    {
        if (parentTransform) {
            vec3 parentRot = parentTransform->getWorldRotation();
            localRotation = rot - parentRot;
        } else {
            localRotation = rot;
        }
        markDirty();
    }

    // ===== Convenience Methods =====

    /**
     * @brief Get model matrix (for rendering)
     */
    mat4 getModelMatrix() const
    {
        return getWorldMatrix();
    }

    /**
     * @brief Move in world space
     */
    void translate(const vec3& offset)
    {
        setWorldPosition(getWorldPosition() + offset);
    }

    /**
     * @brief Rotate in world space
     */
    void rotate(const vec3& eulerAngles)
    {
        setWorldRotation(getWorldRotation() + eulerAngles);
    }

    /**
     * @brief Set position (world space)
     */
    void setPosition(const vec3& pos)
    {
        setWorldPosition(pos);
    }

    /**
     * @brief Set rotation (world space)
     */
    void setRotation(const vec3& rot)
    {
        setWorldRotation(rot);
    }

    /**
     * @brief Set scale (world space)
     */
    void setScale(const vec3& scl)
    {
        localScale = scl;
        markDirty();
    }

    /**
     * @brief Get forward direction (world space)
     */
    vec3 forward() const
    {
        mat4 rot = mat4::euler(getWorldRotation());
        return normalize(rot.transformDirection(vec3::forward));
    }

    /**
     * @brief Get right direction (world space)
     */
    vec3 right() const
    {
        mat4 rot = mat4::euler(getWorldRotation());
        return normalize(rot.transformDirection(vec3::right));
    }

    /**
     * @brief Get up direction (world space)
     */
    vec3 up() const
    {
        mat4 rot = mat4::euler(getWorldRotation());
        return normalize(rot.transformDirection(vec3::up));
    }
};

#endif //ENGINE_TRANSFORM_COMPONENT_H
