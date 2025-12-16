#ifndef MESHFILTER_H
#define MESHFILTER_H

#include "component.h"
#include "../../Rendering/Primitives/mesh.h"
#include <memory>

/**
 * @class MeshFilter
 * @brief Component that holds a reference to a mesh
 * 
 * Separates mesh data from rendering logic.
 * Used together with MeshRenderer component.
 */
class MeshFilter : public Component
{
private:
    std::shared_ptr<Mesh> meshData;

public:
    MeshFilter()
        : meshData(nullptr)
    {
    }

    /**
     * Set the mesh for this filter
     */
    void setMesh(std::shared_ptr<Mesh> mesh)
    {
        meshData = mesh;
    }

    /**
     * Get the mesh from this filter
     */
    std::shared_ptr<Mesh> getMesh() const
    {
        return meshData;
    }

    /**
     * Get raw mesh pointer (for rendering)
     */
    Mesh* getMeshPtr() const
    {
        return meshData.get();
    }

    /**
     * Check if this filter has a mesh
     */
    bool hasMesh() const
    {
        return meshData != nullptr;
    }
};

#endif //MESHFILTER_H
