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
#include "render_types.h"
#include "uniform_buffer.h"
#include "render_command.h"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <algorithm>

// Configuration constants
namespace RenderConfig
{
    constexpr int MAX_LIGHTS = 8;  // Maximum lights supported in shaders
}

/**
 * @struct MeshBuffer
 * @brief GPU buffer data for a mesh
 * Stores VAO, VBO, EBO handles and metadata
 */
struct MeshBuffer
{
    GLuint VAO;             // Vertex Array Object
    GLuint VBO;             // Vertex Buffer Object
    GLuint EBO;             // Element Buffer Object
    size_t indexCount;      // Number of indices
    uint64_t meshID;        // Mesh unique ID
    BufferUsage usage;      // Usage hint
    
    MeshBuffer()
        : VAO(0), VBO(0), EBO(0), indexCount(0), meshID(0), usage(BufferUsage::Static)
    {
    }
};

/**
 * @struct RenderState
 * @brief Caches current OpenGL state to avoid redundant calls
 */
struct RenderState
{
    GLuint boundVAO;
    GLuint boundShader;
    Material* boundMaterial;
    bool depthTestEnabled;
    bool cullFaceEnabled;
    bool wireframeMode;
    
    RenderState()
        : boundVAO(0), boundShader(0), boundMaterial(nullptr),
          depthTestEnabled(false), cullFaceEnabled(false), wireframeMode(false)
    {
    }
    
    void reset()
    {
        boundVAO = 0;
        boundShader = 0;
        boundMaterial = nullptr;
    }
};

/**
 * @class OpenGLRenderer
 * @brief Optimized hardware-accelerated renderer using OpenGL
 * 
 * Features:
 * - Uniform Buffer Objects (UBOs) for camera/lights
 * - Command-based rendering with batching
 * - Mesh ID tracking with dirty flags
 * - Optimized packed vertex format (32 bytes vs 44 bytes)
 * - State caching to minimize GL calls
 * - Static/Dynamic/Streaming buffer hints
 */
class OpenGLRenderer
{
private:
    std::shared_ptr<Shader> activeShader;
    std::unordered_map<uint64_t, MeshBuffer> meshBuffers;  // By mesh ID
    bool initialized;
    
    // Uniform Buffer Objects
    std::unique_ptr<UniformBuffer<CameraUBO>> cameraUBO;
    std::unique_ptr<UniformBuffer<LightsUBO>> lightsUBO;
    
    // Command queue
    DrawCommandQueue renderQueue;
    
    // State caching
    RenderState currentState;
    
    /**
     * Convert Vertex to PackedVertex format (27% smaller)
     */
    PackedVertex packVertex(const Vertex& v)
    {
        PackedVertex pv;
        
        // Position (full precision)
        pv.position[0] = v.position.x;
        pv.position[1] = v.position.y;
        pv.position[2] = v.position.z;
        
        // Normal (octahedron packed)
        packNormal(v.normal.x, v.normal.y, v.normal.z, pv.normal);
        
        // UV (half float)
        pv.uv[0] = floatToHalf(v.uv.x);
        pv.uv[1] = floatToHalf(v.uv.y);
        
        // Color (8-bit per channel)
        pv.color[0] = static_cast<uint8_t>(std::clamp(v.vertexColor.x * 255.0f, 0.0f, 255.0f));
        pv.color[1] = static_cast<uint8_t>(std::clamp(v.vertexColor.y * 255.0f, 0.0f, 255.0f));
        pv.color[2] = static_cast<uint8_t>(std::clamp(v.vertexColor.z * 255.0f, 0.0f, 255.0f));
        pv.color[3] = 255; // Alpha
        
        // Padding for future use
        pv.padding[0] = 0.0f;
        pv.padding[1] = 0.0f;
        
        return pv;
    }

    /**
     * Upload mesh data to GPU buffers (optimized format)
     */
    void uploadMesh(const Mesh& mesh, MeshBuffer& buffer)
    {
        // Pack vertices into optimized format
        std::vector<PackedVertex> packedVertices;
        packedVertices.reserve(mesh.vertices.size());
        
        for (const auto& v : mesh.vertices)
        {
            packedVertices.push_back(packVertex(v));
        }

        // Prepare index data
        std::vector<unsigned int> indices;
        indices.reserve(mesh.triangles.size() * 3);

        for (const auto& tri : mesh.triangles)
        {
            indices.push_back(tri.v0);
            indices.push_back(tri.v1);
            indices.push_back(tri.v2);
        }

        buffer.indexCount = indices.size();
        buffer.meshID = mesh.getID();
        buffer.usage = mesh.getUsage();

        // Create VAO
        glGenVertexArrays(1, &buffer.VAO);
        glBindVertexArray(buffer.VAO);

        // Create and fill VBO
        glGenBuffers(1, &buffer.VBO);
        glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
        glBufferData(GL_ARRAY_BUFFER, 
                     packedVertices.size() * sizeof(PackedVertex), 
                     packedVertices.data(), 
                     toGLUsage(buffer.usage));

        // Create and fill EBO
        glGenBuffers(1, &buffer.EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                     indices.size() * sizeof(unsigned int), 
                     indices.data(), 
                     toGLUsage(buffer.usage));

        // Configure vertex attributes for PackedVertex format
        
        // Position (location = 0): vec3 float
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PackedVertex), 
                              (void*)offsetof(PackedVertex, position));
        glEnableVertexAttribArray(0);

        // Normal (location = 1): 2x int16 (normalized to [-1, 1])
        glVertexAttribPointer(1, 2, GL_SHORT, GL_TRUE, sizeof(PackedVertex), 
                              (void*)offsetof(PackedVertex, normal));
        glEnableVertexAttribArray(1);

        // UV (location = 2): 2x uint16 half float
        glVertexAttribPointer(2, 2, GL_HALF_FLOAT, GL_FALSE, sizeof(PackedVertex), 
                              (void*)offsetof(PackedVertex, uv));
        glEnableVertexAttribArray(2);

        // Color (location = 3): 4x uint8 (normalized to [0, 1])
        glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(PackedVertex), 
                              (void*)offsetof(PackedVertex, color));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);
    }
    
    /**
     * Update mesh buffer if dirty
     */
    void updateMeshIfDirty(const Mesh& mesh, MeshBuffer& buffer)
    {
        if (!mesh.getDirty())
            return;
            
        // Re-upload vertex and index data
        std::vector<PackedVertex> packedVertices;
        packedVertices.reserve(mesh.vertices.size());
        
        for (const auto& v : mesh.vertices)
        {
            packedVertices.push_back(packVertex(v));
        }
        
        std::vector<unsigned int> indices;
        indices.reserve(mesh.triangles.size() * 3);
        
        for (const auto& tri : mesh.triangles)
        {
            indices.push_back(tri.v0);
            indices.push_back(tri.v1);
            indices.push_back(tri.v2);
        }
        
        buffer.indexCount = indices.size();
        
        glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
        glBufferData(GL_ARRAY_BUFFER, 
                     packedVertices.size() * sizeof(PackedVertex), 
                     packedVertices.data(), 
                     toGLUsage(buffer.usage));
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                     indices.size() * sizeof(unsigned int), 
                     indices.data(), 
                     toGLUsage(buffer.usage));
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    /**
     * Bind VAO with state caching
     */
    void bindVAO(GLuint vao)
    {
        if (currentState.boundVAO != vao)
        {
            glBindVertexArray(vao);
            currentState.boundVAO = vao;
        }
    }
    
    /**
     * Use shader with state caching
     */
    void useShader(GLuint shaderID)
    {
        if (currentState.boundShader != shaderID)
        {
            glUseProgram(shaderID);
            currentState.boundShader = shaderID;
        }
    }

public:
    /**
     * @brief Constructor - initializes member variables
     */
    OpenGLRenderer()
        : activeShader(nullptr), initialized(false)
    {
    }

    /**
     * @brief Destructor - cleans up GPU resources
     */
    ~OpenGLRenderer()
    {
        cleanup();
    }

    /**
     * @brief Initialize renderer with default shader and UBOs
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
        
        // Bind UBO blocks in default shader
        activeShader->use();
        activeShader->bindUniformBlock("CameraData", UBOBindings::CAMERA);
        activeShader->bindUniformBlock("LightData", UBOBindings::LIGHTS);

        // Create UBOs
        cameraUBO = std::make_unique<UniformBuffer<CameraUBO>>(UBOBindings::CAMERA);
        lightsUBO = std::make_unique<UniformBuffer<LightsUBO>>(UBOBindings::LIGHTS);

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        currentState.depthTestEnabled = true;

        // Disable backface culling by default (can be enabled per-material)
        // Ground planes and other double-sided geometry need this off
        glDisable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);  // Counter-clockwise winding = front face
        currentState.cullFaceEnabled = false;

        initialized = true;
        std::cout << "OpenGL Renderer initialized (optimized pipeline)" << std::endl;
        std::cout << "  - Packed vertex format: 32 bytes/vertex (27% reduction)" << std::endl;
        std::cout << "  - UBOs enabled for camera/lights" << std::endl;
        std::cout << "  - Command-based rendering active" << std::endl;
        std::cout << "  - State caching enabled" << std::endl;
        return true;
    }

    /**
     * @brief Set custom shader for rendering
     * @param shader Compiled shader program
     */
    void setShader(std::shared_ptr<Shader> shader)
    {
        if (shader && shader->isValid())
        {
            activeShader = shader;
            
            // Bind UBO blocks in new shader
            activeShader->use();
            activeShader->bindUniformBlock("CameraData", UBOBindings::CAMERA);
            activeShader->bindUniformBlock("LightData", UBOBindings::LIGHTS);
        }
    }

    /**
     * @brief Get current active shader
     */
    std::shared_ptr<Shader> getShader() const
    {
        return activeShader;
    }

    /**
     * @brief Clean up all GPU resources
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
        
        // UBOs cleaned up by unique_ptr
        cameraUBO.reset();
        lightsUBO.reset();

        activeShader.reset();
        initialized = false;
    }

    /**
     * Clear framebuffer
     */
    void clear(float r = 0.1f, float g = 0.1f, float b = 0.15f)
    {
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    /**
     * @brief Begin frame - update camera and lights to UBOs
     * @param camera Camera for view/projection
     * @param lights Array of scene lights
     */
    void beginFrame(const Camera& camera, const std::vector<Light>& lights)
    {
        // Update camera UBO
        auto& camData = cameraUBO->get();
        camData.view = camera.getViewMatrix();
        camData.projection = camera.getProjectionMatrix();
        camData.viewProjection = camData.projection * camData.view;
        camData.position = camera.position;
        cameraUBO->upload();
        
        // Update lights UBO
        auto& lightData = lightsUBO->get();
        lightData.numLights = std::min((int)lights.size(), RenderConfig::MAX_LIGHTS);
        
        for (int i = 0; i < lightData.numLights; i++)
        {
            lightData.lights[i].position = lights[i].position;
            lightData.lights[i].type = lights[i].type == Light::Type::Directional ? 0 : 1;
            lightData.lights[i].direction = lights[i].direction;
            lightData.lights[i].intensity = lights[i].intensity;
            lightData.lights[i].color = lights[i].color;
        }
        lightsUBO->upload();
        
        // Clear render queue
        renderQueue.clear();
    }
    
    /**
     * @brief Submit mesh for rendering (command-based)
     * @param mesh Mesh to draw
     * @param modelMatrix Model transformation
     * @param material Material (nullptr = default)
     */
    void submit(const Mesh& mesh, const mat4& modelMatrix, Material* material = nullptr)
    {
        // Calculate depth for sorting (extract translation from matrix)
        // Matrix is row-major in our engine, translation is in row 3
        vec3 worldPos(modelMatrix.m[3][0], modelMatrix.m[3][1], modelMatrix.m[3][2]);
        
        // Use Z depth for sorting
        float depth = worldPos.z;
        
        renderQueue.submit(&mesh, material, modelMatrix, depth);
    }

    /**
     * @brief Flush render queue - execute all submitted commands
     */
    void flush()
    {
        if (!initialized || renderQueue.empty())
            return;
        
        // Get camera/light data for legacy uniforms
        auto& camData = cameraUBO->get();
        auto& lightData = lightsUBO->get();
        
        // Sort commands for optimal batching
        renderQueue.sort();
        
        const auto& commands = renderQueue.getCommands();
        
        // Track last bound states to minimize changes
        Material* lastMaterial = nullptr;
        const Mesh* lastMesh = nullptr;
        
        for (const auto& cmd : commands)
        {
            if (!cmd.mesh)
                continue;
            
            // Get or create mesh buffer
            uint64_t meshID = cmd.mesh->getID();
            auto it = meshBuffers.find(meshID);
            
            if (it == meshBuffers.end())
            {
                // Upload new mesh
                MeshBuffer buffer;
                uploadMesh(*cmd.mesh, buffer);
                meshBuffers[meshID] = buffer;
                it = meshBuffers.find(meshID);
                const_cast<Mesh*>(cmd.mesh)->clearDirty();
            }
            else if (cmd.mesh->getDirty())
            {
                // Re-upload dirty mesh
                updateMeshIfDirty(*cmd.mesh, it->second);
                const_cast<Mesh*>(cmd.mesh)->clearDirty();
            }
            
            const MeshBuffer& buffer = it->second;
            
            // Bind material/shader (minimize state changes)
            std::shared_ptr<Shader> shaderToUse;
            if (cmd.material != lastMaterial)
            {
                if (cmd.material && cmd.material->getShader() && cmd.material->getShader()->isValid())
                {
                    shaderToUse = cmd.material->getShader();
                    useShader(shaderToUse->getID());
                    
                    // Apply material properties - check for validity first
                    try {
                        cmd.material->applyToShader();
                    } catch (...) {
                        // Fallback to default shader if material application fails
                        shaderToUse = activeShader;
                        if (activeShader && activeShader->isValid())
                        {
                            useShader(activeShader->getID());
                            activeShader->use();
                        }
                        else
                        {
                            continue; // Skip this draw if no valid shader
                        }
                    }
                }
                else
                {
                    shaderToUse = activeShader;
                    if (activeShader && activeShader->isValid())
                    {
                        useShader(activeShader->getID());
                        activeShader->use();
                    }
                    else
                    {
                        continue; // Skip this draw if no valid shader
                    }
                }
                
                // Set legacy uniforms for backward compatibility (if shader doesn't use UBOs)
                shaderToUse->setMat4("view", camData.view);
                shaderToUse->setMat4("projection", camData.projection);
                shaderToUse->setVec3("viewPos", camData.position);
                
                // Set light uniforms
                shaderToUse->setInt("numLights", lightData.numLights);
                for (int i = 0; i < lightData.numLights; i++)
                {
                    std::string base = "lights[" + std::to_string(i) + "]";
                    shaderToUse->setInt(base + ".type", lightData.lights[i].type);
                    shaderToUse->setVec3(base + ".position", lightData.lights[i].position);
                    shaderToUse->setVec3(base + ".direction", lightData.lights[i].direction);
                    shaderToUse->setVec3(base + ".color", lightData.lights[i].color);
                    shaderToUse->setFloat(base + ".intensity", lightData.lights[i].intensity);
                }
                
                // Simple lighting uniforms (for Standard/Unlit shaders)
                if (lightData.numLights > 0)
                {
                    shaderToUse->setVec3("lightDir", lightData.lights[0].direction);
                    shaderToUse->setVec3("lightColor", lightData.lights[0].color);
                    shaderToUse->setVec3("ambientColor", vec3(0.1f, 0.1f, 0.15f));
                }
                
                lastMaterial = cmd.material;
            }
            else
            {
                // Reuse last shader
                shaderToUse = (cmd.material && cmd.material->getShader()) ? cmd.material->getShader() : activeShader;
            }
            
            // Skip if no valid shader
            if (!shaderToUse || !shaderToUse->isValid())
                continue;
            
            // Set model matrix (per-object uniform)
            shaderToUse->setMat4("model", cmd.modelMatrix);
            
            // Bind VAO and draw (cached)
            bindVAO(buffer.VAO);
            glDrawElements(GL_TRIANGLES, buffer.indexCount, GL_UNSIGNED_INT, 0);
            
            lastMesh = cmd.mesh;
        }
        
        // Unbind VAO
        bindVAO(0);
        
        // Clear queue for next frame
        renderQueue.clear();
    }
    
    /**
     * @brief Legacy immediate draw (for backward compatibility)
     * Prefer submit() + flush() for better performance
     */
    void drawMesh(const Mesh& mesh, const mat4& modelMatrix, const Camera& camera, const std::vector<Light>& lights)
    {
        drawMesh(mesh, nullptr, modelMatrix, camera, lights);
    }

    /**
     * @brief Legacy immediate draw with material (for backward compatibility)
     */
    void drawMesh(const Mesh& mesh, Material* material, const mat4& modelMatrix, const Camera& camera, const std::vector<Light>& lights)
    {
        // Use command-based rendering under the hood
        beginFrame(camera, lights);
        submit(mesh, modelMatrix, material);
        flush();
    }

    /**
     * Enable/disable wireframe rendering mode
     */
    void setWireframeMode(bool enabled)
    {
        if (currentState.wireframeMode != enabled)
        {
            if (enabled)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            currentState.wireframeMode = enabled;
        }
    }
    
    /**
     * Enable/disable backface culling
     */
    void setCulling(bool enabled)
    {
        if (currentState.cullFaceEnabled != enabled)
        {
            if (enabled)
            {
                glEnable(GL_CULL_FACE);
            }
            else
            {
                glDisable(GL_CULL_FACE);
            }
            currentState.cullFaceEnabled = enabled;
        }
    }

    /**
     * Check if renderer is initialized
     */
    bool isInitialized() const { return initialized; }
    
    /**
     * Get number of cached mesh buffers
     */
    size_t getMeshBufferCount() const { return meshBuffers.size(); }
    
    /**
     * Get number of pending draw commands
     */
    size_t getPendingCommandCount() const { return renderQueue.size(); }
};

#endif //OPENGL_RENDERER_H
