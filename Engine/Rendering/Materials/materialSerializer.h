#ifndef MATERIAL_SERIALIZER_H
#define MATERIAL_SERIALIZER_H

#include "material.h"
#include "builtin_materials.h"
#include "../Loaders/textureLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>

/**
 * @class MaterialSerializer
 * @brief Saves and loads materials from .mat files with extensible type system
 * 
 * Material file format (.mat):
 * materialType: <TypeName>
 * <propertyType>: <name> <values...>
 * 
 * Supported property types:
 * - color: name r g b
 * - float: name value
 * - int: name value
 * - vec3: name x y z
 * - texture: name filepath
 * 
 * Example:
 * materialType: Standard
 * color: _Color 1.0 0.0 0.0
 * float: _Metallic 0.8
 * float: _Smoothness 0.9
 * texture: _MainTex Assets/Textures/diffuse.png
 * 
 * Custom material types can be registered using registerMaterialType()
 */
class MaterialSerializer
{
public:
    using MaterialFactory = std::function<std::shared_ptr<Material>()>;

private:
    // Registry of material type factories
    static std::unordered_map<std::string, MaterialFactory>& getMaterialRegistry()
    {
        static std::unordered_map<std::string, MaterialFactory> registry;
        static bool initialized = false;
        
        if (!initialized) {
            // Register built-in material types
            registry["Standard"] = []() { return BuiltinMaterials::createStandard(); };
            registry["Unlit"] = []() { return BuiltinMaterials::createUnlit(); };
            registry["StandardSpecular"] = []() { return BuiltinMaterials::createStandardSpecular(); };
            initialized = true;
        }
        
        return registry;
    }

public:
    /**
     * @brief Register a custom material type
     * @param typeName Name of the material type (used in .mat files)
     * @param factory Factory function that creates the material
     * 
     * Example:
     * MaterialSerializer::registerMaterialType("CustomPBR", []() {
     *     return std::make_shared<Material>(customShader, "CustomPBR");
     * });
     */
    static void registerMaterialType(const std::string& typeName, MaterialFactory factory)
    {
        getMaterialRegistry()[typeName] = factory;
    }
    /**
     * @brief Save material to file
     * @param material Material to save
     * @param filepath Path to .mat file
     * @return true if successful
     * 
     * Automatically serializes all properties from the material's property maps.
     * Works with any material type - no hardcoding needed.
     */
    static bool saveToFile(const Material& material, const std::string& filepath)
    {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to save material to: " << filepath << std::endl;
            return false;
        }

        // Write material type
        file << "materialType: " << material.getName() << "\n";
        file << "\n";  // Blank line for readability
        
        // Write all color properties
        const auto& colors = material.getColorProperties();
        if (!colors.empty()) {
            file << "# Color Properties\n";
            for (const auto& [name, value] : colors) {
                file << "color: " << name << " " 
                     << value.x << " " << value.y << " " << value.z << "\n";
            }
            file << "\n";
        }
        
        // Write all float properties
        const auto& floats = material.getFloatProperties();
        if (!floats.empty()) {
            file << "# Float Properties\n";
            for (const auto& [name, value] : floats) {
                file << "float: " << name << " " << value << "\n";
            }
            file << "\n";
        }
        
        // Write all int properties
        const auto& ints = material.getIntProperties();
        if (!ints.empty()) {
            file << "# Int Properties\n";
            for (const auto& [name, value] : ints) {
                file << "int: " << name << " " << value << "\n";
            }
            file << "\n";
        }
        
        // Write all vec3 properties
        const auto& vectors = material.getVectorProperties();
        if (!vectors.empty()) {
            file << "# Vector Properties\n";
            for (const auto& [name, value] : vectors) {
                file << "vec3: " << name << " " 
                     << value.x << " " << value.y << " " << value.z << "\n";
            }
            file << "\n";
        }

        // Write all texture properties
        const auto& texturePaths = material.getTexturePaths();
        if (!texturePaths.empty()) {
            file << "# Texture Properties\n";
            for (const auto& [name, path] : texturePaths) {
                file << "texture: " << name << " " << path << "\n";
            }
        }

        file.close();
        return true;
    }

    /**
     * @brief Load material from file
     * @param filepath Path to .mat file
     * @return Loaded material or nullptr if failed
     * 
     * Automatically deserializes all properties into the material.
     * Supports any registered material type via the registry system.
     */
    static std::shared_ptr<Material> loadFromFile(const std::string& filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to load material from: " << filepath << std::endl;
            return nullptr;
        }

        std::shared_ptr<Material> material;
        std::string line;
        std::string materialType;

        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;

            std::istringstream iss(line);
            std::string key;
            iss >> key;

            if (key == "materialType:") {
                iss >> materialType;
                
                // Look up material type in registry
                auto& registry = getMaterialRegistry();
                auto it = registry.find(materialType);
                
                if (it != registry.end()) {
                    material = it->second();  // Create material using factory
                } else {
                    std::cerr << "Unknown material type: " << materialType << std::endl;
                    std::cerr << "Available types: ";
                    for (const auto& [name, _] : registry) {
                        std::cerr << name << " ";
                    }
                    std::cerr << std::endl;
                    return nullptr;
                }
            }
            else if (material) {
                // Parse property line
                if (key == "color:") {
                    std::string name;
                    float r, g, b;
                    iss >> name >> r >> g >> b;
                    material->setColor(name, color(r, g, b));
                }
                else if (key == "float:") {
                    std::string name;
                    float value;
                    iss >> name >> value;
                    material->setFloat(name, value);
                }
                else if (key == "int:") {
                    std::string name;
                    int value;
                    iss >> name >> value;
                    material->setInt(name, value);
                }
                else if (key == "vec3:") {
                    std::string name;
                    float x, y, z;
                    iss >> name >> x >> y >> z;
                    material->setVector(name, vec3(x, y, z));
                }
                else if (key == "texture:") {
                    std::string name, filepath;
                    iss >> name >> filepath;
                    
                    // Load texture from file
                    auto texture = TextureLoader::loadFromFile(filepath);
                    if (texture) {
                        material->setTexture(name, texture, filepath);
                    } else {
                        std::cerr << "Failed to load texture: " << filepath << std::endl;
                    }
                }
                else {
                    std::cerr << "Unknown property type: " << key << std::endl;
                }
            }
            else {
                std::cerr << "Property '" << key << "' defined before materialType" << std::endl;
            }
        }

        file.close();
        
        if (!material) {
            std::cerr << "No material type found in file: " << filepath << std::endl;
        }
        
        return material;
    }
};

#endif
