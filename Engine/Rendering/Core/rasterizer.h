//
// Created by Graphics Engine
//

#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "framebuffer.h"
#include "../Primitives/mesh.h"
#include "../camera.h"
#include "../light.h"
#include "../../Math/mat4.h"
#include <algorithm>
#include <cmath>

/**
 * @struct Fragment
 * @brief Represents a fragment (pixel candidate) during rasterization
 */
struct Fragment
{
    vec3 position;
    vec3 normal;
    vec3 worldPos;
    color vertexColor;
    float depth;
};

/**
 * @class Rasterizer
 * @brief Software rasterizer for rendering 3D meshes onto a framebuffer
 */
class Rasterizer
{
public:
    /** 
     * @enum RenderMode
     * @brief Different rendering modes for the rasterizer
     */
    enum class RenderMode
    {
        Wireframe,
        Solid,
        SolidWireframe
    };

    RenderMode renderMode;
    bool backfaceCulling;
    color wireframeColor;

    /**
     * @brief Construct a new Rasterizer object
     */
    Rasterizer()
        : renderMode(RenderMode::Solid),
          backfaceCulling(true),
          wireframeColor(1, 1, 1) {}

    /**
     * @brief Draw a mesh onto the framebuffer
     * @param fb Framebuffer to draw onto
     * @param mesh Mesh to render
     * @param modelMatrix Model transformation matrix
     * @param camera Camera for view and projection
     * @param lights Scene lights for shading
     */
    void drawMesh(Framebuffer& fb, const Mesh& mesh, const mat4& modelMatrix, 
                  const Camera& camera, const std::vector<Light>& lights)
    {
        mat4 mvp = camera.getViewProjectionMatrix() * modelMatrix;
        mat4 mv = camera.getViewMatrix() * modelMatrix;

        // Transform vertices
        std::vector<vec3> transformedPositions;
        std::vector<vec3> clipSpacePositions;
        std::vector<vec3> screenPositions;
        std::vector<vec3> transformedNormals;
        std::vector<vec3> worldPositions;

        transformedPositions.reserve(mesh.vertices.size());
        clipSpacePositions.reserve(mesh.vertices.size());
        screenPositions.reserve(mesh.vertices.size());
        transformedNormals.reserve(mesh.vertices.size());
        worldPositions.reserve(mesh.vertices.size());

        // Transform all vertices
        for (const auto& vertex : mesh.vertices)
        {
            // World position
            vec3 worldPos = modelMatrix.transformPoint(vertex.position);
            worldPositions.push_back(worldPos);

            // Clip space (with w component)
            vec3 clipPos = mvp.transformPoint(vertex.position);
            clipSpacePositions.push_back(clipPos);

            // View space (for lighting)
            vec3 viewPos = mv.transformPoint(vertex.position);
            transformedPositions.push_back(viewPos);

            // Transform normal
            vec3 worldNormal = modelMatrix.transformDirection(vertex.normal.normalized());
            transformedNormals.push_back(worldNormal);

            // Screen space with proper depth mapping
            // NDC is in range [-1, 1], map depth to [0, 1] for depth buffer
            vec3 ndc = clipPos;
            float depth = (ndc.z + 1.0f) * 0.5f; // Map from [-1,1] to [0,1]
            
            vec3 screen(
                (ndc.x + 1.0f) * 0.5f * fb.width,
                (1.0f - ndc.y) * 0.5f * fb.height,
                depth // Use properly mapped depth
            );
            screenPositions.push_back(screen);
        }

        // Draw triangles
        for (const auto& tri : mesh.triangles)
        {
            // Backface culling
            if (backfaceCulling)
            {
                vec3 v0 = screenPositions[tri.v0];
                vec3 v1 = screenPositions[tri.v1];
                vec3 v2 = screenPositions[tri.v2];

                vec3 edge1(v1.x - v0.x, v1.y - v0.y, 0);
                vec3 edge2(v2.x - v0.x, v2.y - v0.y, 0);
                
                float crossZ = edge1.x * edge2.y - edge1.y * edge2.x;
                if (crossZ <= 0) continue; // Back-facing
            }

            if (renderMode == RenderMode::Wireframe || renderMode == RenderMode::SolidWireframe)
            {
                drawWireframeTriangle(fb, 
                    screenPositions[tri.v0],
                    screenPositions[tri.v1],
                    screenPositions[tri.v2]);
            }

            if (renderMode == RenderMode::Solid || renderMode == RenderMode::SolidWireframe)
            {
                drawFilledTriangle(fb,
                    screenPositions[tri.v0], screenPositions[tri.v1], screenPositions[tri.v2],
                    transformedNormals[tri.v0], transformedNormals[tri.v1], transformedNormals[tri.v2],
                    worldPositions[tri.v0], worldPositions[tri.v1], worldPositions[tri.v2],
                    mesh.vertices[tri.v0].vertexColor, 
                    mesh.vertices[tri.v1].vertexColor, 
                    mesh.vertices[tri.v2].vertexColor,
                    camera, lights);
            }
        }
    }

private:
    // Bresenham's line algorithm
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    void drawLine(Framebuffer& fb, int x0, int y0, int x1, int y1, const color& col)
    {
        int dx = std::abs(x1 - x0);
        int dy = std::abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;

        while (true)
        {
            fb.setPixel(x0, y0, col);

            if (x0 == x1 && y0 == y1) break;

            int e2 = 2 * err;
            if (e2 > -dy)
            {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx)
            {
                err += dx;
                y0 += sy;
            }
        }
    }

    void drawWireframeTriangle(Framebuffer& fb, const vec3& v0, const vec3& v1, const vec3& v2)
    {
        drawLine(fb, (int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y, wireframeColor);
        drawLine(fb, (int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y, wireframeColor);
        drawLine(fb, (int)v2.x, (int)v2.y, (int)v0.x, (int)v0.y, wireframeColor);
    }

    // Barycentric coordinates
    vec3 barycentric(const vec3& p, const vec3& a, const vec3& b, const vec3& c)
    {
        vec3 v0(c.x - a.x, b.x - a.x, a.x - p.x);
        vec3 v1(c.y - a.y, b.y - a.y, a.y - p.y);
        
        vec3 u = vec3::cross(v0,  v1);
        
        if (std::abs(u.z) < 1.0f)
            return vec3(-1, 1, 1);
        
        return vec3(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
    }

    void drawFilledTriangle(Framebuffer& fb,
                           const vec3& v0, const vec3& v1, const vec3& v2,
                           const vec3& n0, const vec3& n1, const vec3& n2,
                           const vec3& w0, const vec3& w1, const vec3& w2,
                           const color& c0, const color& c1, const color& c2,
                           const Camera& camera, const std::vector<Light>& lights)
    {
        // Bounding box
        int minX = std::max(0, (int)std::min({v0.x, v1.x, v2.x}));
        int maxX = std::min(fb.width - 1, (int)std::max({v0.x, v1.x, v2.x}));
        int minY = std::max(0, (int)std::min({v0.y, v1.y, v2.y}));
        int maxY = std::min(fb.height - 1, (int)std::max({v0.y, v1.y, v2.y}));

        // Rasterize
        for (int y = minY; y <= maxY; y++)
        {
            for (int x = minX; x <= maxX; x++)
            {
                vec3 p(x + 0.5f, y + 0.5f, 0);
                vec3 bc = barycentric(p, v0, v1, v2);

                if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;

                // Interpolate depth
                float depth = bc.x * v0.z + bc.y * v1.z + bc.z * v2.z;

                // Depth test
                if (depth >= fb.getDepth(x, y)) continue;

                // Interpolate attributes
                vec3 normal = bc.x * n0 + bc.y * n1 + bc.z * n2.normalized();
                vec3 worldPos = bc.x * w0 + bc.y * w1 + bc.z * w2;
                color baseColor = bc.x * c0 + bc.y * c1 + bc.z * c2;

                // Apply lighting
                color finalColor = calculateLighting(worldPos, normal, baseColor, camera.position, lights);

                fb.setPixelWithDepth(x, y, depth, finalColor);
            }
        }
    }

    color calculateLighting(const vec3& worldPos, const vec3& normal, const color& baseColor,
                           const vec3& cameraPos, const std::vector<Light>& lights)
    {
        if (lights.empty())
            return baseColor;

        color ambient = color(0.1f, 0.1f, 0.1f); // Ambient light: what little light is always present
        color diffuse(0, 0, 0); // Diffuse light: light scattered in many directions
        color specular(0, 0, 0); // Specular light: shiny highlights

        vec3 viewDir = cameraPos - worldPos.normalized();

        for (const auto& light : lights)
        {
            vec3 lightDir;
            float attenuation = 1.0f;

            if (light.type == Light::Type::Directional)
            {
                lightDir = -light.direction.normalized();
            }
            else if (light.type == Light::Type::Point)
            {
                vec3 toLight = light.position - worldPos;
                float distance = toLight.length();
                lightDir = toLight.normalized();
                attenuation = 1.0f / (1.0f + 0.09f * distance + 0.032f * distance * distance);
            }

            // Diffuse
            float diff = std::max(vec3::dot(normal,  lightDir), 0.0f);
            diffuse += light.color * light.intensity * diff * attenuation;

            // Specular (Blinn-Phong)
            vec3 halfDir = lightDir + viewDir.normalized();
            float spec = std::pow(std::max(vec3::dot(normal,  halfDir), 0.0f), 32.0f);
            specular += light.color * light.intensity * spec * attenuation * 0.5f;
        }

        color result = baseColor * (ambient + diffuse) + specular;
        
        // Clamp to [0, 1]
        result[0] = std::min(1.0f, std::max(0.0f, result.x));
        result[1] = std::min(1.0f, std::max(0.0f, result.y));
        result[2] = std::min(1.0f, std::max(0.0f, result.z));

        return result;
    }
};

#endif //RASTERIZER_H
