//
// Created by Graphics Engine
//

#ifndef MESH_H
#define MESH_H

#include "../../Math/vec3.h"
#include "../../Math/vec2.h"
#include "../color.h"
#include "../Core/render_types.h"
#include <vector>
#include <memory>
#include <atomic>

struct Vertex
{
    vec3 position;
    vec3 normal;
    vec3 uv; // Using vec3 but only x,y used for texture coords
    color vertexColor;

    Vertex() : position(vec3::zero), normal(vec3::up), uv(vec3::zero), vertexColor(1, 1, 1) {}
    
    Vertex(const vec3& pos, const vec3& norm = vec3::up, const vec3& texCoord = vec3::zero, const color& col = color(1, 1, 1))
        : position(pos), normal(norm), uv(texCoord), vertexColor(col) {}
    
    // Texture coordinate accessors (for convenience with vec2)
    vec2 getTexCoord() const { return vec2(uv.x, uv.y); }
    void setTexCoord(const vec2& tc) { uv.x = tc.x; uv.y = tc.y; }
};

struct Triangle
{
    int v0, v1, v2; // Indices into vertex array

    Triangle() : v0(0), v1(0), v2(0) {}
    Triangle(int a, int b, int c) : v0(a), v1(b), v2(c) {}
};

class Mesh
{
private:
    static std::atomic<uint64_t> nextMeshID;
    uint64_t meshID;
    bool isDirty;
    BufferUsage usage;
    
public:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    Mesh(BufferUsage bufferUsage = BufferUsage::Static) 
        : meshID(++nextMeshID), isDirty(true), usage(bufferUsage)
    {
    }
    
    /**
     * @brief Get unique mesh identifier
     */
    uint64_t getID() const { return meshID; }
    
    /**
     * @brief Check if mesh data has changed since last upload
     */
    bool getDirty() const { return isDirty; }
    
    /**
     * @brief Mark mesh as clean (called by renderer after upload)
     */
    void clearDirty() { isDirty = false; }
    
    /**
     * @brief Mark mesh as dirty (needs re-upload)
     */
    void markDirty() { isDirty = true; }
    
    /**
     * @brief Get buffer usage hint
     */
    BufferUsage getUsage() const { return usage; }
    
    /**
     * @brief Set buffer usage hint (marks dirty)
     */
    void setUsage(BufferUsage newUsage)
    {
        if (usage != newUsage)
        {
            usage = newUsage;
            isDirty = true;
        }
    }

    // Create a cube mesh
    static std::shared_ptr<Mesh> createCube(float size = 1.0f, BufferUsage usage = BufferUsage::Static)
    {
        auto mesh = std::make_shared<Mesh>(usage);
        float half = size * 0.5f;

        // 8 vertices of a cube
        mesh->vertices = {
            // Front face (z+)
            Vertex(vec3(-half, -half, half), vec3(0, 0, 1), vec3(0, 0, 0), color(1, 0, 0)),
            Vertex(vec3(half, -half, half), vec3(0, 0, 1), vec3(1, 0, 0), color(0, 1, 0)),
            Vertex(vec3(half, half, half), vec3(0, 0, 1), vec3(1, 1, 0), color(0, 0, 1)),
            Vertex(vec3(-half, half, half), vec3(0, 0, 1), vec3(0, 1, 0), color(1, 1, 0)),
            
            // Back face (z-)
            Vertex(vec3(half, -half, -half), vec3(0, 0, -1), vec3(0, 0, 0), color(1, 0, 1)),
            Vertex(vec3(-half, -half, -half), vec3(0, 0, -1), vec3(1, 0, 0), color(0, 1, 1)),
            Vertex(vec3(-half, half, -half), vec3(0, 0, -1), vec3(1, 1, 0), color(1, 1, 1)),
            Vertex(vec3(half, half, -half), vec3(0, 0, -1), vec3(0, 1, 0), color(0.5f, 0.5f, 0.5f)),
            
            // Right face (x+)
            Vertex(vec3(half, -half, half), vec3(1, 0, 0), vec3(0, 0, 0), color(1, 0, 0)),
            Vertex(vec3(half, -half, -half), vec3(1, 0, 0), vec3(1, 0, 0), color(0, 1, 0)),
            Vertex(vec3(half, half, -half), vec3(1, 0, 0), vec3(1, 1, 0), color(0, 0, 1)),
            Vertex(vec3(half, half, half), vec3(1, 0, 0), vec3(0, 1, 0), color(1, 1, 0)),
            
            // Left face (x-)
            Vertex(vec3(-half, -half, -half), vec3(-1, 0, 0), vec3(0, 0, 0), color(1, 0, 1)),
            Vertex(vec3(-half, -half, half), vec3(-1, 0, 0), vec3(1, 0, 0), color(0, 1, 1)),
            Vertex(vec3(-half, half, half), vec3(-1, 0, 0), vec3(1, 1, 0), color(1, 1, 1)),
            Vertex(vec3(-half, half, -half), vec3(-1, 0, 0), vec3(0, 1, 0), color(0.5f, 0.5f, 0.5f)),
            
            // Top face (y+)
            Vertex(vec3(-half, half, half), vec3(0, 1, 0), vec3(0, 0, 0), color(1, 0, 0)),
            Vertex(vec3(half, half, half), vec3(0, 1, 0), vec3(1, 0, 0), color(0, 1, 0)),
            Vertex(vec3(half, half, -half), vec3(0, 1, 0), vec3(1, 1, 0), color(0, 0, 1)),
            Vertex(vec3(-half, half, -half), vec3(0, 1, 0), vec3(0, 1, 0), color(1, 1, 0)),
            
            // Bottom face (y-)
            Vertex(vec3(-half, -half, -half), vec3(0, -1, 0), vec3(0, 0, 0), color(1, 0, 1)),
            Vertex(vec3(half, -half, -half), vec3(0, -1, 0), vec3(1, 0, 0), color(0, 1, 1)),
            Vertex(vec3(half, -half, half), vec3(0, -1, 0), vec3(1, 1, 0), color(1, 1, 1)),
            Vertex(vec3(-half, -half, half), vec3(0, -1, 0), vec3(0, 1, 0), color(0.5f, 0.5f, 0.5f)),
        };

        // 12 triangles (2 per face)
        mesh->triangles = {
            // Front
            Triangle(0, 1, 2), Triangle(2, 3, 0),
            // Back
            Triangle(4, 5, 6), Triangle(6, 7, 4),
            // Right
            Triangle(8, 9, 10), Triangle(10, 11, 8),
            // Left
            Triangle(12, 13, 14), Triangle(14, 15, 12),
            // Top
            Triangle(16, 17, 18), Triangle(18, 19, 16),
            // Bottom
            Triangle(20, 21, 22), Triangle(22, 23, 20)
        };

        return mesh;
    }

    // Create a plane mesh
    static std::shared_ptr<Mesh> createPlane(float width = 10.0f, float height = 10.0f, BufferUsage usage = BufferUsage::Static)
    {
        auto mesh = std::make_shared<Mesh>(usage);
        float hw = width * 0.5f;
        float hh = height * 0.5f;

        // Create a subdivided plane (2x2 grid = 4 quads = 8 triangles)
        // This eliminates the visible seam artifact from single-quad planes
        const int subdivisionsX = 2;
        const int subdivisionsZ = 2;
        
        // Generate vertices
        for (int z = 0; z <= subdivisionsZ; z++)
        {
            for (int x = 0; x <= subdivisionsX; x++)
            {
                float px = -hw + (x * width / subdivisionsX);
                float pz = -hh + (z * height / subdivisionsZ);
                float u = x / (float)subdivisionsX;
                float v = z / (float)subdivisionsZ;
                
                mesh->vertices.push_back(
                    Vertex(vec3(px, 0, pz), vec3(0, 1, 0), vec3(u, v, 0), color(1.0f, 1.0f, 1.0f))
                );
            }
        }
        
        // Generate triangles (2 per quad)
        for (int z = 0; z < subdivisionsZ; z++)
        {
            for (int x = 0; x < subdivisionsX; x++)
            {
                int i0 = z * (subdivisionsX + 1) + x;
                int i1 = i0 + 1;
                int i2 = i0 + (subdivisionsX + 1);
                int i3 = i2 + 1;
                
                // CCW winding from above
                mesh->triangles.push_back(Triangle(i0, i1, i2));
                mesh->triangles.push_back(Triangle(i1, i3, i2));
            }
        }

        return mesh;
    }

    // Create a sphere mesh (icosphere approximation)
    static std::shared_ptr<Mesh> createSphere(float radius = 1.0f, int subdivisions = 2, BufferUsage usage = BufferUsage::Static)
    {
        auto mesh = std::make_shared<Mesh>(usage);
        
        // Create icosahedron
        float t = (1.0f + std::sqrt(5.0f)) / 2.0f;
        
        mesh->vertices = {
            Vertex(vec3(-1, t, 0).normalized() * radius, vec3(0, 1, 0)),
            Vertex(vec3(1, t, 0).normalized() * radius, vec3(0, 1, 0)),
            Vertex(vec3(-1, -t, 0).normalized() * radius, vec3(0, -1, 0)),
            Vertex(vec3(1, -t, 0).normalized() * radius, vec3(0, -1, 0)),
            
            Vertex(vec3(0, -1, t).normalized() * radius, vec3(0, 0, 1)),
            Vertex(vec3(0, 1, t).normalized() * radius, vec3(0, 0, 1)),
            Vertex(vec3(0, -1, -t).normalized() * radius, vec3(0, 0, -1)),
            Vertex(vec3(0, 1, -t).normalized() * radius, vec3(0, 0, -1)),
            
            Vertex(vec3(t, 0, -1).normalized() * radius, vec3(1, 0, 0)),
            Vertex(vec3(t, 0, 1).normalized() * radius, vec3(1, 0, 0)),
            Vertex(vec3(-t, 0, -1).normalized() * radius, vec3(-1, 0, 0)),
            Vertex(vec3(-t, 0, 1).normalized() * radius, vec3(-1, 0, 0))
        };

        mesh->triangles = {
            Triangle(0, 11, 5), Triangle(0, 5, 1), Triangle(0, 1, 7), Triangle(0, 7, 10), Triangle(0, 10, 11),
            Triangle(1, 5, 9), Triangle(5, 11, 4), Triangle(11, 10, 2), Triangle(10, 7, 6), Triangle(7, 1, 8),
            Triangle(3, 9, 4), Triangle(3, 4, 2), Triangle(3, 2, 6), Triangle(3, 6, 8), Triangle(3, 8, 9),
            Triangle(4, 9, 5), Triangle(2, 4, 11), Triangle(6, 2, 10), Triangle(8, 6, 7), Triangle(9, 8, 1)
        };

        // Update normals based on sphere positions
        for (auto& v : mesh->vertices)
        {
            v.normal = v.position.normalized();
            v.vertexColor = color(0.8f, 0.3f, 0.3f);
        }

        return mesh;
    }

    void calculateNormals()
    {
        // Reset all normals
        for (auto& v : vertices)
            v.normal = vec3::zero;

        // Accumulate face normals
        for (const auto& tri : triangles)
        {
            vec3 v0 = vertices[tri.v0].position;
            vec3 v1 = vertices[tri.v1].position;
            vec3 v2 = vertices[tri.v2].position;

            vec3 normal = vec3::cross(v1 - v0, v2 - v0).normalized();

            vertices[tri.v0].normal += normal;
            vertices[tri.v1].normal += normal;
            vertices[tri.v2].normal += normal;
        }

        // Normalize
        for (auto& v : vertices)
        {
            if (v.normal.lengthSquared() > 0.001f)
                v.normal = v.normal.normalized();
        }
        
        markDirty(); // Normals changed
    }
};

// Initialize static mesh ID counter
std::atomic<uint64_t> Mesh::nextMeshID{0};

#endif //MESH_H
