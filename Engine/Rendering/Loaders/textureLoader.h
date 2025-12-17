//
// TextureLoader - Load textures from image files
//

#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "../texture.h"
#include <memory>
#include <string>
#include <iostream>
#include <algorithm>
#include <cstring>

// STB Image - only include implementation once with guard
#ifndef STB_IMAGE_IMPLEMENTATION_INCLUDED
#define STB_IMAGE_IMPLEMENTATION_INCLUDED
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

/**
 * @class TextureLoader
 * @brief Loads textures from image files (PNG, JPG, BMP, etc.)
 * 
 * Uses STB Image library to load various image formats.
 * Automatically handles format conversion and texture creation.
 * Supports automatic texture resizing for mismatched dimensions.
 */
class TextureLoader
{
private:
    /**
     * @brief Resize image data using bilinear interpolation
     * Proper game engine approach - handles any size mismatch
     * 
     * @param inputData Source pixel data
     * @param inputWidth Source width
     * @param inputHeight Source height
     * @param outputWidth Target width
     * @param outputHeight Target height
     * @param channels Number of color channels (3 for RGB, 4 for RGBA)
     * @return Resized pixel data (must be freed with delete[])
     */
    static unsigned char* resizeImage(
        const unsigned char* inputData,
        int inputWidth, int inputHeight,
        int outputWidth, int outputHeight,
        int channels)
    {
        // No resize needed
        if (inputWidth == outputWidth && inputHeight == outputHeight)
        {
            unsigned char* result = new unsigned char[inputWidth * inputHeight * channels];
            std::memcpy(result, inputData, inputWidth * inputHeight * channels);
            return result;
        }

        unsigned char* outputData = new unsigned char[outputWidth * outputHeight * channels];
        
        // Use bilinear interpolation for quality scaling
        for (int y = 0; y < outputHeight; y++)
        {
            for (int x = 0; x < outputWidth; x++)
            {
                // Map output coordinates to input coordinates
                float srcX = (x + 0.5f) * (float)inputWidth / outputWidth - 0.5f;
                float srcY = (y + 0.5f) * (float)inputHeight / outputHeight - 0.5f;
                
                // Clamp to valid range
                srcX = std::max(0.0f, std::min(srcX, (float)inputWidth - 1.001f));
                srcY = std::max(0.0f, std::min(srcY, (float)inputHeight - 1.001f));
                
                int x0 = (int)srcX;
                int y0 = (int)srcY;
                int x1 = std::min(x0 + 1, inputWidth - 1);
                int y1 = std::min(y0 + 1, inputHeight - 1);
                
                float fx = srcX - x0;
                float fy = srcY - y0;
                
                // Bilinear interpolation for each channel
                for (int c = 0; c < channels; c++)
                {
                    float v00 = inputData[(y0 * inputWidth + x0) * channels + c];
                    float v10 = inputData[(y0 * inputWidth + x1) * channels + c];
                    float v01 = inputData[(y1 * inputWidth + x0) * channels + c];
                    float v11 = inputData[(y1 * inputWidth + x1) * channels + c];
                    
                    // Interpolate
                    float v0 = v00 * (1 - fx) + v10 * fx;
                    float v1 = v01 * (1 - fx) + v11 * fx;
                    float v = v0 * (1 - fy) + v1 * fy;
                    
                    outputData[(y * outputWidth + x) * channels + c] = (unsigned char)(v + 0.5f);
                }
            }
        }
        
        return outputData;
    }

public:
    /**
     * @brief Load texture from file with automatic resizing to target dimensions
     * @param filepath Path to image file
     * @param targetWidth Target width (0 = use image's natural width)
     * @param targetHeight Target height (0 = use image's natural height)
     * @param filter Filtering mode
     * @param wrap Wrap mode
     * @return Loaded and resized texture, or nullptr if failed
     */
    static std::shared_ptr<Texture> loadFromFile(
        const std::string& filepath,
        int targetWidth = 0,
        int targetHeight = 0,
        Texture::FilterMode filter = Texture::FilterMode::Bilinear,
        Texture::WrapMode wrap = Texture::WrapMode::Repeat)
    {
        int width, height, channels;
        
        // Load image data
        stbi_set_flip_vertically_on_load(true);
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
        
        unsigned char* dataToUse = data;
        unsigned char* resizedData = nullptr;
        int finalWidth = width;
        int finalHeight = height;
        
        // Resize if target dimensions specified and different from source
        if ((targetWidth > 0 || targetHeight > 0) && 
            (targetWidth != width || targetHeight != height))
        {
            int newWidth = (targetWidth > 0) ? targetWidth : width;
            int newHeight = (targetHeight > 0) ? targetHeight : height;
            
            std::cout << "  Resizing to: " << newWidth << "x" << newHeight << std::endl;
            
            resizedData = resizeImage(data, width, height, newWidth, newHeight, channels);
            
            if (resizedData)
            {
                dataToUse = resizedData;
                finalWidth = newWidth;
                finalHeight = newHeight;
            }
            else
            {
                std::cerr << "  Resize failed, using original dimensions" << std::endl;
                dataToUse = data;
            }
        }
        
        // Create texture
        auto texture = std::make_shared<Texture>();
        bool success = texture->createFromData(dataToUse, finalWidth, finalHeight, channels, filter, wrap);
        
        // Free image data
        stbi_image_free(data);
        if (resizedData)
        {
            delete[] resizedData;
        }
        
        if (!success)
        {
            std::cerr << "Failed to create OpenGL texture from: " << filepath << std::endl;
            return nullptr;
        }
        
        return texture;
    }
    
    /**
     * @brief Load texture from file, automatically scaling to match reference dimensions
     * @param filepath Path to image file
     * @param referenceWidth Width to match
     * @param referenceHeight Height to match
     * @param filter Filtering mode
     * @param wrap Wrap mode
     * @return Loaded and resized texture matching reference dimensions
     */
    static std::shared_ptr<Texture> loadFromFileWithReference(
        const std::string& filepath,
        int referenceWidth,
        int referenceHeight,
        Texture::FilterMode filter = Texture::FilterMode::Bilinear,
        Texture::WrapMode wrap = Texture::WrapMode::Repeat)
    {
        return loadFromFile(filepath, referenceWidth, referenceHeight, filter, wrap);
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
