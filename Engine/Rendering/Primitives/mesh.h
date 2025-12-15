//
// Created by Graphics Engine
//

#ifndef MESH_H
#define MESH_H

#include "../../Math/vec3.h"
#include "../color.h"
#include <vector>
#include <memory>

struct Vertex
{
    vec3 position;
    vec3 normal;
    vec3 uv; // Using vec3 but only x,y used for texture coords
    color vertexColor;

    Vertex() : position(vec3::zero), normal(vec3::up), uv(vec3::zero), vertexColor(1, 1, 1) {}
    
    Vertex(const vec3& pos, const vec3& norm = vec3::up, const vec3& texCoord = vec3::zero, const color& col = color(1, 1, 1))
        : position(pos), normal(norm), uv(texCoord), vertexColor(col) {}
};

struct Triangle
{
    int v0, v1, v2; // Indices into vertex array

    Triangle() : v0(0), v1(0), v2(0) {}
    Triangle(int a, int b, int c) : v0(a), v1(b), v2(c) {}
};

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    Mesh() = default;

    // Create a cube mesh
    static std::shared_ptr<Mesh> createCube(float size = 1.0f)
    {
        auto mesh = std::make_shared<Mesh>();
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
    static std::shared_ptr<Mesh> createPlane(float width = 10.0f, float height = 10.0f)
    {
        auto mesh = std::make_shared<Mesh>();
        float hw = width * 0.5f;
        float hh = height * 0.5f;

        mesh->vertices = {
            Vertex(vec3(-hw, 0, -hh), vec3(0, 1, 0), vec3(0, 0, 0), color(0.7f, 0.7f, 0.7f)),
            Vertex(vec3(hw, 0, -hh), vec3(0, 1, 0), vec3(1, 0, 0), color(0.7f, 0.7f, 0.7f)),
            Vertex(vec3(hw, 0, hh), vec3(0, 1, 0), vec3(1, 1, 0), color(0.7f, 0.7f, 0.7f)),
            Vertex(vec3(-hw, 0, hh), vec3(0, 1, 0), vec3(0, 1, 0), color(0.7f, 0.7f, 0.7f))
        };

        mesh->triangles = {
            Triangle(0, 1, 2),  // CCW from above
            Triangle(0, 2, 3)   // CCW from above
        };

        return mesh;
    }

    // Create a sphere mesh (icosphere approximation)
    static std::shared_ptr<Mesh> createSphere(float radius = 1.0f, int subdivisions = 2)
    {
        auto mesh = std::make_shared<Mesh>();
        
        // Create icosahedron
        float t = (1.0f + std::sqrt(5.0f)) / 2.0f;
        
        mesh->vertices = {
            Vertex(normalize(vec3(-1, t, 0)) * radius, vec3(0, 1, 0)),
            Vertex(normalize(vec3(1, t, 0)) * radius, vec3(0, 1, 0)),
            Vertex(normalize(vec3(-1, -t, 0)) * radius, vec3(0, -1, 0)),
            Vertex(normalize(vec3(1, -t, 0)) * radius, vec3(0, -1, 0)),
            
            Vertex(normalize(vec3(0, -1, t)) * radius, vec3(0, 0, 1)),
            Vertex(normalize(vec3(0, 1, t)) * radius, vec3(0, 0, 1)),
            Vertex(normalize(vec3(0, -1, -t)) * radius, vec3(0, 0, -1)),
            Vertex(normalize(vec3(0, 1, -t)) * radius, vec3(0, 0, -1)),
            
            Vertex(normalize(vec3(t, 0, -1)) * radius, vec3(1, 0, 0)),
            Vertex(normalize(vec3(t, 0, 1)) * radius, vec3(1, 0, 0)),
            Vertex(normalize(vec3(-t, 0, -1)) * radius, vec3(-1, 0, 0)),
            Vertex(normalize(vec3(-t, 0, 1)) * radius, vec3(-1, 0, 0))
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
            v.normal = normalize(v.position);
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

            vec3 normal = normalize(cross(v1 - v0, v2 - v0));

            vertices[tri.v0].normal += normal;
            vertices[tri.v1].normal += normal;
            vertices[tri.v2].normal += normal;
        }

        // Normalize
        for (auto& v : vertices)
        {
            if (v.normal.sqr_magnitude() > 0.001f)
                v.normal = normalize(v.normal);
        }
    }
};

#endif //MESH_H
