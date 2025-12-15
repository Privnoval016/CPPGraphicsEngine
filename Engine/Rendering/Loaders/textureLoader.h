//
// TextureLoader - Load textures from image files
//

#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "../texture.h"
#include <memory>
#include <string>
#include <iostream>

// STB Image implementation
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/**
 * @class TextureLoader
 * @brief Loads textures from image files (PNG, JPG, BMP, etc.)
 * 
 * Uses STB Image library to load various image formats.
 * Automatically handles format conversion and texture creation.
 */
class TextureLoader
{
public:
    /**
     * @brief Load texture from image file
     * @param filepath Path to image file
     * @param filter Filtering mode for the texture
     * @param wrap Wrap mode for the texture
     * @return Loaded texture or nullptr if failed
     */
    static std::shared_ptr<Texture> loadFromFile(
        const std::string& filepath,
        Texture::FilterMode filter = Texture::FilterMode::Bilinear,
        Texture::WrapMode wrap = Texture::WrapMode::Repeat)
    {
        int width, height, channels;
        
        // Load image data
        stbi_set_flip_vertically_on_load(true);  // OpenGL expects bottom-left origin
        unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
        
        if (!data)
        {
            std::cerr << "Failed to load texture: " << filepath << std::endl;
            std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
            return nullptr;
        }
        
        std::cout << "Loaded texture: " << filepath << std::endl;
        std::cout << "  Size: " << width << "x" << height << std::endl;
        std::cout << "  Channels: " << channels << std::endl;
        
        // Create texture
        auto texture = std::make_shared<Texture>();
        bool success = texture->createFromData(data, width, height, channels, filter, wrap);
        
        // Free image data
        stbi_image_free(data);
        
        if (!success)
        {
            std::cerr << "Failed to create OpenGL texture from: " << filepath << std::endl;
            return nullptr;
        }
        
        return texture;
    }
    
    /**
     * @brief Check if a file format is supported
     * @param filepath Path to check
     * @return true if format is likely supported
     */
    static bool isSupportedFormat(const std::string& filepath)
    {
        std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
        
        // Convert to lowercase
        for (char& c : ext)
        {
            c = std::tolower(c);
        }
        
        return (ext == "png" || ext == "jpg" || ext == "jpeg" || 
                ext == "bmp" || ext == "tga" || ext == "psd" || 
                ext == "gif" || ext == "hdr" || ext == "pic");
    }
};

#endif // TEXTURE_LOADER_H
