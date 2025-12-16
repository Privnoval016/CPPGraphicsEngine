#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "../../Math/mat4.h"
#include "../Primitives/mesh.h"
#include "../Materials/material.h"
#include <vector>
#include <memory>

/**
 * @file render_command.h
 * @brief Command-based rendering system for batching and sorting
 * 
 * Instead of immediate draw calls, commands are submitted to a queue,
 * then sorted and batched to minimize state changes.
 */

/**
 * @struct RenderCommand
 * @brief Single draw command with all required state
 */
struct RenderCommand
{
    const Mesh* mesh;              // Mesh to draw
    Material* material;            // Material (nullptr = default)
    mat4 modelMatrix;              // Model transformation
    uint64_t sortKey;              // For batching and sorting
    
    RenderCommand()
        : mesh(nullptr), material(nullptr), modelMatrix(mat4::identity()), sortKey(0)
    {
    }
    
    /**
     * @brief Generate sort key for batching
     * Key layout (64 bits):
     * - Bits 63-48: Material/Shader ID (16 bits)
     * - Bits 47-32: Mesh ID (16 bits)
     * - Bits 31-0:  Depth (for transparency sorting)
     */
    static uint64_t generateSortKey(const Mesh* mesh, Material* mat, float depth = 0.0f)
    {
        uint64_t key = 0;
        
        // Material ID (higher bits = sorted first)
        if (mat && mat->getShader())
        {
            uint64_t matID = static_cast<uint64_t>(mat->getShader()->getID()) & 0xFFFF;
            key |= (matID << 48);
        }
        
        // Mesh ID (for batching same meshes)
        if (mesh)
        {
            uint64_t meshID = mesh->getID() & 0xFFFF;
            key |= (meshID << 32);
        }
        
        // Depth (for front-to-back or back-to-front sorting)
        uint32_t depthBits = *reinterpret_cast<const uint32_t*>(&depth);
        key |= depthBits;
        
        return key;
    }
};

/**
 * @class DrawCommandQueue
 * @brief Queue for collecting and sorting render commands
 */
class DrawCommandQueue
{
private:
    std::vector<RenderCommand> commands;
    bool needsSort;
    
public:
    DrawCommandQueue()
        : needsSort(false)
    {
        commands.reserve(1024); // Pre-allocate for performance
    }
    
    /**
     * @brief Submit a render command
     */
    void submit(const RenderCommand& cmd)
    {
        commands.push_back(cmd);
        needsSort = true;
    }
    
    /**
     * @brief Submit a render command (convenience)
     */
    void submit(const Mesh* mesh, Material* mat, const mat4& model, float depth = 0.0f)
    {
        RenderCommand cmd;
        cmd.mesh = mesh;
        cmd.material = mat;
        cmd.modelMatrix = model;
        cmd.sortKey = RenderCommand::generateSortKey(mesh, mat, depth);
        commands.push_back(cmd);
        needsSort = true;
    }
    
    /**
     * @brief Sort commands by sort key (minimizes state changes)
     */
    void sort()
    {
        if (needsSort)
        {
            std::sort(commands.begin(), commands.end(),
                [](const RenderCommand& a, const RenderCommand& b)
                {
                    return a.sortKey < b.sortKey;
                });
            needsSort = false;
        }
    }
    
    /**
     * @brief Get all commands (sorted if needed)
     */
    const std::vector<RenderCommand>& getCommands()
    {
        if (needsSort)
            sort();
        return commands;
    }
    
    /**
     * @brief Clear all commands
     */
    void clear()
    {
        commands.clear();
        needsSort = false;
    }
    
    /**
     * @brief Get number of commands
     */
    size_t size() const { return commands.size(); }
    
    /**
     * @brief Check if queue is empty
     */
    bool empty() const { return commands.empty(); }
};

#endif // RENDER_COMMAND_H
