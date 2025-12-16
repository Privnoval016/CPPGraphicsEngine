#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "component.h"
#include "meshFilter.h"
#include "../../Rendering/Materials/material.h"
#include <memory>

/**
 * @class MeshRenderer
 * @brief Component that renders a mesh with a material
 * 
 * Works together with MeshFilter component.
 * Holds material reference and rendering settings.
 */
class MeshRenderer : public Component
{
private:
    std::shared_ptr<Material> materialInstance;
    bool castShadows;
    bool receiveShadows;
    bool enabled;

public:
    MeshRenderer()
        : materialInstance(nullptr),
          castShadows(true),
          receiveShadows(true),
          enabled(true)
    {
    }

    /**
     * Set the material for rendering
     */
    void setMaterial(std::shared_ptr<Material> mat)
    {
        materialInstance = mat;
    }

    /**
     * Get the material
     */
    std::shared_ptr<Material> getMaterial() const
    {
        return materialInstance;
    }

    /**
     * Get raw material pointer (for rendering)
     */
    Material* getMaterialPtr() const
    {
        return materialInstance.get();
    }

    /**
     * Check if this renderer has a material
     */
    bool hasMaterial() const
    {
        return materialInstance != nullptr;
    }

    /**
     * Enable/disable rendering
     */
    void setEnabled(bool value)
    {
        enabled = value;
    }

    bool isEnabled() const
    {
        return enabled;
    }

    /**
     * Enable/disable shadow casting
     */
    void setCastShadows(bool value)
    {
        castShadows = value;
    }

    bool getCastShadows() const
    {
        return castShadows;
    }

    /**
     * Enable/disable shadow receiving
     */
    void setReceiveShadows(bool value)
    {
        receiveShadows = value;
    }

    bool getReceiveShadows() const
    {
        return receiveShadows;
    }

    /**
     * Check if this renderer can render
     * (has MeshFilter with mesh and is enabled)
     */
    bool canRender() const
    {
        if (!enabled || !gameObject)
            return false;

        auto meshFilter = gameObject->getComponent<MeshFilter>();
        return meshFilter && meshFilter->hasMesh();
    }
};

#endif //MESHRENDERER_H
