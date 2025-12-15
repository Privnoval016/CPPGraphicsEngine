//
// Material - Unity-like material system with PBR support
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include "../Shaders/shader.h"
#include "../texture.h"
#include "../../Math/vec3.h"
#include "../color.h"
#include <memory>
#include <string>
#include <unordered_map>

/**
 * @enum RenderQueue
 * @brief Render order for materials (matches Unity)
 */
enum class RenderQueue
{
    Background = 1000,
    Geometry = 2000,
    AlphaTest = 2450,
    Transparent = 3000,
    Overlay = 4000
};

/**
 * @class Material
 * @brief Unity-like material system
 * 
 * Materials combine shaders with properties (colors, textures, floats, etc.)
 * Supports both built-in materials (Standard, Unlit) and custom shaders.
 */
class Material
{
private:
    std::shared_ptr<Shader> shader;
    std::string name;
    RenderQueue renderQueue;
    
    // Material properties
    std::unordered_map<std::string, float> floatProperties;
    std::unordered_map<std::string, vec3> vectorProperties;
    std::unordered_map<std::string, color> colorProperties;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textureProperties;
    std::unordered_map<std::string, int> intProperties;
    
    // Default white texture for unbound samplers (prevents OpenGL warnings)
    static std::shared_ptr<Texture> getDefaultTexture()
    {
        static std::shared_ptr<Texture> defaultTex;
        if (!defaultTex)
        {
            defaultTex = std::make_shared<Texture>();
            defaultTex->createSolidColor(1.0f, 1.0f, 1.0f, 1.0f);  // White
        }
        return defaultTex;
    }

public:
    Material()
        : name("Material"), renderQueue(RenderQueue::Geometry)
    {
    }

    Material(std::shared_ptr<Shader> shaderProgram, const std::string& materialName = "Material")
        : shader(shaderProgram), name(materialName), renderQueue(RenderQueue::Geometry)
    {
    }

    // ==================== Shader Management ====================
    
    void setShader(std::shared_ptr<Shader> shaderProgram)
    {
        shader = shaderProgram;
    }

    std::shared_ptr<Shader> getShader() const
    {
        return shader;
    }

    // ==================== Property Setters ====================
    
    void setFloat(const std::string& propertyName, float value)
    {
        floatProperties[propertyName] = value;
    }

    void setInt(const std::string& propertyName, int value)
    {
        intProperties[propertyName] = value;
    }

    void setVector(const std::string& propertyName, const vec3& value)
    {
        vectorProperties[propertyName] = value;
    }

    void setColor(const std::string& propertyName, const color& value)
    {
        colorProperties[propertyName] = value;
    }

    void setTexture(const std::string& propertyName, std::shared_ptr<Texture> texture)
    {
        textureProperties[propertyName] = texture;
    }

    // ==================== Property Getters ====================
    
    float getFloat(const std::string& propertyName, float defaultValue = 0.0f) const
    {
        auto it = floatProperties.find(propertyName);
        return (it != floatProperties.end()) ? it->second : defaultValue;
    }

    int getInt(const std::string& propertyName, int defaultValue = 0) const
    {
        auto it = intProperties.find(propertyName);
        return (it != intProperties.end()) ? it->second : defaultValue;
    }

    vec3 getVector(const std::string& propertyName, const vec3& defaultValue = vec3::zero) const
    {
        auto it = vectorProperties.find(propertyName);
        return (it != vectorProperties.end()) ? it->second : defaultValue;
    }

    color getColor(const std::string& propertyName, const color& defaultValue = color(1,1,1)) const
    {
        auto it = colorProperties.find(propertyName);
        return (it != colorProperties.end()) ? it->second : defaultValue;
    }

    std::shared_ptr<Texture> getTexture(const std::string& propertyName) const
    {
        auto it = textureProperties.find(propertyName);
        return (it != textureProperties.end()) ? it->second : nullptr;
    }

    // ==================== Render Queue ====================
    
    void setRenderQueue(RenderQueue queue)
    {
        renderQueue = queue;
    }

    RenderQueue getRenderQueue() const
    {
        return renderQueue;
    }

    // ==================== Material Application ====================
    
    /**
     * Apply all material properties to the shader
     * Call this before rendering with this material
     */
    void applyToShader()
    {
        if (!shader || !shader->isValid())
            return;

        shader->use();

        // Apply float properties
        for (const auto& [name, value] : floatProperties)
        {
            shader->setFloat(name, value);
        }

        // Apply int properties
        for (const auto& [name, value] : intProperties)
        {
            shader->setInt(name, value);
        }

        // Apply vector properties
        for (const auto& [name, value] : vectorProperties)
        {
            shader->setVec3(name, value);
        }

        // Apply color properties
        for (const auto& [name, value] : colorProperties)
        {
            shader->setColor(name, value);
        }

        // Apply texture properties
        int textureUnit = 0;
        
        // List of common texture samplers (bind default texture if not set)
        std::vector<std::string> samplerNames = {
            "_MainTex", "_MetallicGlossMap", "_BumpMap", "_OcclusionMap"
        };
        
        for (const std::string& samplerName : samplerNames)
        {
            auto it = textureProperties.find(samplerName);
            if (it != textureProperties.end() && it->second && it->second->isLoaded())
            {
                // Use custom texture
                it->second->bind(textureUnit);
                shader->setInt(samplerName, textureUnit);
            }
            else
            {
                // Bind default white texture to prevent warnings
                getDefaultTexture()->bind(textureUnit);
                shader->setInt(samplerName, textureUnit);
            }
            textureUnit++;
        }
    }

    // ==================== Utility ====================
    
    void setName(const std::string& materialName)
    {
        name = materialName;
    }

    std::string getName() const
    {
        return name;
    }

    /**
     * Create a copy of this material
     */
    std::shared_ptr<Material> clone() const
    {
        auto copy = std::make_shared<Material>(shader, name + " (Clone)");
        copy->renderQueue = renderQueue;
        copy->floatProperties = floatProperties;
        copy->vectorProperties = vectorProperties;
        copy->colorProperties = colorProperties;
        copy->textureProperties = textureProperties;
        copy->intProperties = intProperties;
        return copy;
    }

    // ==================== Property Getters (for serialization) ====================
    
    const std::unordered_map<std::string, float>& getFloatProperties() const { return floatProperties; }
    const std::unordered_map<std::string, vec3>& getVectorProperties() const { return vectorProperties; }
    const std::unordered_map<std::string, color>& getColorProperties() const { return colorProperties; }
    const std::unordered_map<std::string, std::shared_ptr<Texture>>& getTextureProperties() const { return textureProperties; }
    const std::unordered_map<std::string, int>& getIntProperties() const { return intProperties; }
};

#endif //MATERIAL_H
