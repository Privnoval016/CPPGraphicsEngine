//
// OpenGL Renderer - Hardware-accelerated rendering backend
// Uses GPU for fast triangle rasterization with lighting and custom shaders
//

#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "../Primitives/mesh.h"
#include "../camera.h"
#include "../light.h"
#include "../Shaders/shader.h"
#include "../Shaders/default_shaders.h"
#include "../../Math/mat4.h"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <memory>

// Configuration constants
namespace RenderConfig
{
    constexpr int MAX_LIGHTS = 8;  // Maximum lights supported in shaders
}

/**
 * @struct MeshBuffer
 * @brief GPU buffer data for a mesh
 * Stores VAO, VBO, EBO handles and index count
 */
struct MeshBuffer
{
    GLuint VAO;          // Vertex Array Object - stores vertex attribute configuration
    GLuint VBO;          // Vertex Buffer Object - stores vertex data
    GLuint EBO;          // Element Buffer Object - stores indices
    size_t indexCount;   // Number of indices to draw
};

/**
 * @class OpenGLRenderer
 * @brief Hardware-accelerated renderer using OpenGL
 * 
 * Provides GPU-based rendering with custom shader support.
 * Automatically manages mesh uploads and shader uniforms.
 * Supports multiple lights and custom materials.
 */
class OpenGLRenderer
{
private:
    std::shared_ptr<Shader> activeShader;  // Current shader program
    std::unordered_map<const Mesh*, MeshBuffer> meshBuffers;  // Mesh GPU buffer cache
    bool initialized;

    /**
     * Upload mesh data to GPU buffers
     * @param mesh Source mesh data
     * @param buffer Output buffer handles
     * 
     * Creates VAO, VBO, and EBO. Vertex layout:
     * - Location 0: Position (vec3)
     * - Location 1: Normal (vec3)
     * - Location 2: Color (vec3)
     * - Location 3: TexCoord (vec2)
     */
    void uploadMesh(const Mesh& mesh, MeshBuffer& buffer)
    {
        // Prepare vertex data: position (3) + normal (3) + color (3) + uv (2) = 11 floats per vertex
        std::vector<float> vertices;
        vertices.reserve(mesh.vertices.size() * 11);

        for (const auto& v : mesh.vertices)
        {
            // Position
            vertices.push_back(v.position.x);
            vertices.push_back(v.position.y);
            vertices.push_back(v.position.z);
            // Normal
            vertices.push_back(v.normal.x);
            vertices.push_back(v.normal.y);
            vertices.push_back(v.normal.z);
            // Color
            vertices.push_back(v.vertexColor.x);
            vertices.push_back(v.vertexColor.y);
            vertices.push_back(v.vertexColor.z);
            // UV (only x and y from vec3)
            vertices.push_back(v.uv.x);
            vertices.push_back(v.uv.y);
        }

        // Prepare index data (3 indices per triangle)
        std::vector<unsigned int> indices;
        indices.reserve(mesh.triangles.size() * 3);

        for (const auto& tri : mesh.triangles)
        {
            indices.push_back(tri.v0);
            indices.push_back(tri.v1);
            indices.push_back(tri.v2);
        }

        buffer.indexCount = indices.size();

        // Create and bind VAO (stores vertex attribute configuration)
        glGenVertexArrays(1, &buffer.VAO);
        glBindVertexArray(buffer.VAO);

        // Create and fill VBO (vertex data)
        glGenBuffers(1, &buffer.VBO);
        glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Create and fill EBO (index data)
        glGenBuffers(1, &buffer.EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Configure vertex attributes
        // Position attribute (location = 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute (location = 1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Color attribute (location = 2)
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // TexCoord attribute (location = 3)
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
        glEnableVertexAttribArray(3);

        // Unbind VAO
        glBindVertexArray(0);
    }

public:
    /**
     * Constructor - creates uninitialized renderer
     */
    OpenGLRenderer()
        : activeShader(nullptr), initialized(false)
    {
    }

    /**
     * Destructor - cleans up GPU resources
     */
    ~OpenGLRenderer()
    {
        cleanup();
    }

    /**
     * Initialize renderer with default Blinn-Phong shader
     * Must be called after OpenGL context is created
     * @return true if initialization successful
     */
    bool initialize()
    {
        if (initialized) return true;

        // Create default Blinn-Phong shader
        activeShader = std::make_shared<Shader>();
        if (!activeShader->compileFromSource(
            DefaultShaders::BLINN_PHONG_VERTEX,
            DefaultShaders::BLINN_PHONG_FRAGMENT))
        {
            std::cerr << "Failed to compile default shader" << std::endl;
            return false;
        }

        // Enable depth testing (for proper 3D rendering)
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // Disable backface culling for now (debugging ground plane)
        // TODO: Re-enable once double-sided rendering is fixed
        glDisable(GL_CULL_FACE);
        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_BACK);
        //glFrontFace(GL_CCW);  // Counter-clockwise winding = front face

        initialized = true;
        std::cout << "OpenGL Renderer initialized with Blinn-Phong shader" << std::endl;
        return true;
    }

    /**
     * Set custom shader for rendering
     * @param shader Compiled shader program
     */
    void setShader(std::shared_ptr<Shader> shader)
    {
        if (shader && shader->isValid())
        {
            activeShader = shader;
        }
    }

    /**
     * Get current active shader
     */
    std::shared_ptr<Shader> getShader() const
    {
        return activeShader;
    }

    /**
     * Clean up all GPU resources
     */
    void cleanup()
    {
        if (!initialized) return;

        // Delete all mesh buffers
        for (auto& pair : meshBuffers)
        {
            glDeleteVertexArrays(1, &pair.second.VAO);
            glDeleteBuffers(1, &pair.second.VBO);
            glDeleteBuffers(1, &pair.second.EBO);
        }
        meshBuffers.clear();

        activeShader.reset();
        initialized = false;
    }

    /**
     * Clear framebuffer
     * @param r Red component [0-1]
     * @param g Green component [0-1]
     * @param b Blue component [0-1]
     */
    void clear(float r = 0.1f, float g = 0.1f, float b = 0.15f)
    {
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    /**
     * Draw a mesh with lighting
     * @param mesh Mesh to draw
     * @param modelMatrix Model transformation matrix
     * @param camera Camera for view/projection
     * @param lights Array of scene lights
     */
    void drawMesh(const Mesh& mesh, const mat4& modelMatrix, const Camera& camera, const std::vector<Light>& lights)
    {
        drawMesh(mesh, nullptr, modelMatrix, camera, lights);
    }

    /**
     * Draw a mesh with material and lighting
     * @param mesh Mesh to draw
     * @param material Material to use (nullptr = use default shader)
     * @param modelMatrix Model transformation matrix
     * @param camera Camera for view/projection
     * @param lights Array of scene lights
     */
    void drawMesh(const Mesh& mesh, Material* material, const mat4& modelMatrix, const Camera& camera, const std::vector<Light>& lights)
    {
        if (!initialized) return;

        // Get or create mesh buffer (lazy upload)
        auto it = meshBuffers.find(&mesh);
        if (it == meshBuffers.end())
        {
            MeshBuffer buffer;
            uploadMesh(mesh, buffer);
            meshBuffers[&mesh] = buffer;
            it = meshBuffers.find(&mesh);
        }

        const MeshBuffer& buffer = it->second;

        // Use material shader if provided, otherwise use default
        std::shared_ptr<Shader> shaderToUse = (material && material->getShader()) ? material->getShader() : activeShader;
        
        if (!shaderToUse || !shaderToUse->isValid())
            return;

        // Apply material properties if material is provided
        if (material)
        {
            material->applyToShader();
        }
        else
        {
            shaderToUse->use();
        }

        // Set transformation matrices
        mat4 view = camera.getViewMatrix();
        mat4 proj = camera.getProjectionMatrix();

        shaderToUse->setMat4("model", modelMatrix);
        shaderToUse->setMat4("view", view);
        shaderToUse->setMat4("projection", proj);
        shaderToUse->setVec3("viewPos", camera.position);

        // Set lighting uniforms
        int numLights = std::min((int)lights.size(), RenderConfig::MAX_LIGHTS);
        shaderToUse->setInt("numLights", numLights);

        for (int i = 0; i < numLights; i++)
        {
            std::string base = "lights[" + std::to_string(i) + "]";
            
            shaderToUse->setInt(base + ".type", lights[i].type == Light::Type::Directional ? 0 : 1);
            shaderToUse->setVec3(base + ".position", lights[i].position);
            shaderToUse->setVec3(base + ".direction", lights[i].direction);
            shaderToUse->setColor(base + ".color", lights[i].color);
            shaderToUse->setFloat(base + ".intensity", lights[i].intensity);
        }

        // For built-in materials, set simple light uniforms (for Standard/Unlit shaders)
        if (!lights.empty())
        {
            // Use first light as primary
            shaderToUse->setVec3("lightDir", lights[0].direction);
            shaderToUse->setColor("lightColor", lights[0].color);
            shaderToUse->setVec3("ambientColor", vec3(0.1f, 0.1f, 0.15f));
        }

        // Draw mesh
        glBindVertexArray(buffer.VAO);
        glDrawElements(GL_TRIANGLES, buffer.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    /**
     * Enable/disable wireframe rendering mode
     * @param enabled true for wireframe, false for solid
     */
    void setWireframeMode(bool enabled)
    {
        if (enabled)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    /**
     * Check if renderer is initialized
     */
    bool isInitialized() const { return initialized; }
};

#endif //OPENGL_RENDERER_H
