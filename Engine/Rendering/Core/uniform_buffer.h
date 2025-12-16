#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "../../Math/mat4.h"
#include "../../Math/vec3.h"
#include "../color.h"
#include <vector>

/**
 * @file uniform_buffer.h
 * @brief Uniform Buffer Objects for efficient GPU data uploads
 * 
 * UBOs allow uploading data once and sharing across shaders,
 * reducing glUniform calls significantly.
 */

namespace UBOBindings
{
    // Standard UBO binding points
    constexpr GLuint CAMERA = 0;
    constexpr GLuint LIGHTS = 1;
    constexpr GLuint MATERIAL = 2;
}

/**
 * @struct CameraUBO
 * @brief Camera data for shaders (std140 layout)
 * Binding point: 0
 */
struct CameraUBO
{
    alignas(16) mat4 view;           // 64 bytes
    alignas(16) mat4 projection;     // 64 bytes
    alignas(16) mat4 viewProjection; // 64 bytes
    alignas(16) vec3 position;       // 12 bytes + 4 padding
    alignas(4) float _pad0;          // Padding
    
    static constexpr size_t SIZE = 208;
};

/**
 * @struct LightData
 * @brief Per-light data (std140 layout)
 */
struct LightData
{
    alignas(16) vec3 position;    // 12 bytes + 4 padding
    alignas(4) int type;          // 4 bytes (0=directional, 1=point)
    alignas(16) vec3 direction;   // 12 bytes + 4 padding
    alignas(4) float intensity;   // 4 bytes
    alignas(16) vec3 color;       // 12 bytes + 4 padding
    alignas(4) float _pad0;       // Padding
    
    static constexpr size_t SIZE = 64;
};

/**
 * @struct LightsUBO
 * @brief All scene lights (std140 layout)
 * Binding point: 1
 */
struct LightsUBO
{
    alignas(16) LightData lights[8]; // 512 bytes (8 lights max)
    alignas(16) int numLights;       // 4 bytes + 12 padding
    alignas(4) float _pad[3];        // Padding
    
    static constexpr size_t SIZE = 528;
};

/**
 * @class UniformBuffer
 * @brief Manages a single UBO with automatic updates
 */
template<typename T>
class UniformBuffer
{
private:
    GLuint ubo;
    GLuint bindingPoint;
    T data;
    bool dirty;
    
public:
    UniformBuffer(GLuint binding)
        : ubo(0), bindingPoint(binding), dirty(true)
    {
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, T::SIZE, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    
    ~UniformBuffer()
    {
        if (ubo != 0)
        {
            glDeleteBuffers(1, &ubo);
        }
    }
    
    // Non-copyable
    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;
    
    /**
     * @brief Get reference to data for modification
     */
    T& get()
    {
        dirty = true;
        return data;
    }
    
    /**
     * @brief Upload dirty data to GPU
     */
    void upload()
    {
        if (dirty)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, ubo);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, T::SIZE, &data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            dirty = false;
        }
    }
    
    /**
     * @brief Force upload even if not dirty
     */
    void forceUpload()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, T::SIZE, &data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        dirty = false;
    }
    
    /**
     * @brief Bind this UBO to its binding point
     */
    void bind() const
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);
    }
    
    GLuint getID() const { return ubo; }
    GLuint getBindingPoint() const { return bindingPoint; }
};

#endif // UNIFORM_BUFFER_H
