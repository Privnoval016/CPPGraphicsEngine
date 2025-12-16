#ifndef RENDER_TYPES_H
#define RENDER_TYPES_H

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <cstdint>

/**
 * @file render_types.h
 * @brief Optimized rendering data types and structures
 */

/**
 * @enum BufferUsage
 * @brief Hints for OpenGL buffer usage patterns
 */
enum class BufferUsage
{
    Static,     // GL_STATIC_DRAW - data changes rarely
    Dynamic,    // GL_DYNAMIC_DRAW - data changes occasionally
    Streaming   // GL_STREAM_DRAW - data changes every frame
};

/**
 * @brief Convert BufferUsage to OpenGL enum
 */
inline GLenum toGLUsage(BufferUsage usage)
{
    switch (usage)
    {
        case BufferUsage::Static: return GL_STATIC_DRAW;
        case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
        case BufferUsage::Streaming: return GL_STREAM_DRAW;
        default: return GL_STATIC_DRAW;
    }
}

/**
 * @struct PackedVertex
 * @brief Optimized vertex format (32 bytes per vertex)
 * 
 * Layout:
 * - Position: vec3 (12 bytes)
 * - Normal: 2x 16-bit packed (4 bytes) 
 * - UV: 2x 16-bit half float (4 bytes)
 * - Color: 4x 8-bit RGBA (4 bytes)
 * - Padding: 8 bytes for alignment
 * 
 * Previous format: 44 bytes per vertex (11 floats)
 * New format: 32 bytes per vertex (27% smaller)
 */
struct PackedVertex
{
    float position[3];      // 12 bytes - keep full precision for positions
    int16_t normal[2];      // 4 bytes - octahedron encoding (x, y)
    uint16_t uv[2];         // 4 bytes - half float texture coordinates
    uint8_t color[4];       // 4 bytes - RGBA (0-255)
    float padding[2];       // 8 bytes - future use (tangent, etc.)
};

/**
 * @brief Encode normal to octahedron (2D projection of 3D normal)
 * @param nx, ny, nz Normal components [-1, 1]
 * @param out Output array [2] for packed values
 */
inline void packNormal(float nx, float ny, float nz, int16_t out[2])
{
    // Octahedron normal encoding
    float invL1 = 1.0f / (std::abs(nx) + std::abs(ny) + std::abs(nz));
    float px = nx * invL1;
    float py = ny * invL1;
    
    if (nz < 0.0f)
    {
        float signX = px >= 0.0f ? 1.0f : -1.0f;
        float signY = py >= 0.0f ? 1.0f : -1.0f;
        px = (1.0f - std::abs(py)) * signX;
        py = (1.0f - std::abs(px)) * signY;
    }
    
    // Scale to int16 range
    out[0] = static_cast<int16_t>(px * 32767.0f);
    out[1] = static_cast<int16_t>(py * 32767.0f);
}

/**
 * @brief Decode octahedron normal back to 3D
 * @param packed Input packed normal [2]
 * @param nx, ny, nz Output normal components
 */
inline void unpackNormal(const int16_t packed[2], float& nx, float& ny, float& nz)
{
    // Convert from int16 to [-1, 1]
    float px = packed[0] / 32767.0f;
    float py = packed[1] / 32767.0f;
    
    nz = 1.0f - std::abs(px) - std::abs(py);
    
    if (nz < 0.0f)
    {
        float signX = px >= 0.0f ? 1.0f : -1.0f;
        float signY = py >= 0.0f ? 1.0f : -1.0f;
        float oldPx = px;
        px = (1.0f - std::abs(py)) * signX;
        py = (1.0f - std::abs(oldPx)) * signY;
    }
    
    nx = px;
    ny = py;
    
    // Normalize
    float len = std::sqrt(nx * nx + ny * ny + nz * nz);
    if (len > 0.0f)
    {
        nx /= len;
        ny /= len;
        nz /= len;
    }
}

/**
 * @brief Convert float to half-float (16-bit)
 * Simplified version without proper rounding for performance
 */
inline uint16_t floatToHalf(float f)
{
    // Quick and dirty float to half conversion
    uint32_t bits = *reinterpret_cast<uint32_t*>(&f);
    uint16_t sign = (bits >> 16) & 0x8000;
    int32_t exponent = ((bits >> 23) & 0xFF) - 127 + 15;
    uint32_t mantissa = bits & 0x007FFFFF;
    
    if (exponent <= 0)
    {
        // Underflow
        return sign;
    }
    else if (exponent >= 31)
    {
        // Overflow
        return sign | 0x7C00;
    }
    
    return sign | (exponent << 10) | (mantissa >> 13);
}

/**
 * @brief Convert half-float to float
 */
inline float halfToFloat(uint16_t h)
{
    uint32_t sign = (h & 0x8000) << 16;
    int32_t exponent = (h >> 10) & 0x1F;
    uint32_t mantissa = h & 0x03FF;
    
    if (exponent == 0)
    {
        // Zero or denormalized
        if (mantissa == 0)
        {
            uint32_t bits = sign;
            return *reinterpret_cast<float*>(&bits);
        }
    }
    else if (exponent == 31)
    {
        // Infinity or NaN
        exponent = 255;
    }
    else
    {
        exponent += 127 - 15;
    }
    
    uint32_t bits = sign | (exponent << 23) | (mantissa << 13);
    return *reinterpret_cast<float*>(&bits);
}

#endif // RENDER_TYPES_H
