//
// Texture - Image data for materials
//

#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <string>
#include <iostream>

/**
 * @class Texture
 * @brief Manages OpenGL texture objects
 * 
 * Loads and manages 2D textures for use in materials.
 * Supports various formats and filtering modes.
 */
class Texture
{
private:
    GLuint textureID;
    int width;
    int height;
    int channels;
    bool loaded;

public:
    enum class FilterMode
    {
        Nearest,
        Linear,
        Bilinear,
        Trilinear
    };

    enum class WrapMode
    {
        Repeat,
        Clamp,
        Mirror
    };

    Texture()
        : textureID(0), width(0), height(0), channels(0), loaded(false)
    {
    }

    ~Texture()
    {
        if (textureID != 0)
        {
            glDeleteTextures(1, &textureID);
        }
    }

    /**
     * Create texture from raw data
     * @param data Raw pixel data (RGB or RGBA)
     * @param w Width in pixels
     * @param h Height in pixels
     * @param ch Number of channels (3 for RGB, 4 for RGBA)
     * @param filter Filtering mode
     * @param wrap Wrap mode
     */
    bool createFromData(const unsigned char* data, int w, int h, int ch,
                       FilterMode filter = FilterMode::Bilinear,
                       WrapMode wrap = WrapMode::Repeat)
    {
        width = w;
        height = h;
        channels = ch;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set wrap mode
        GLenum wrapMode = GL_REPEAT;
        switch (wrap)
        {
            case WrapMode::Repeat: wrapMode = GL_REPEAT; break;
            case WrapMode::Clamp: wrapMode = GL_CLAMP_TO_EDGE; break;
            case WrapMode::Mirror: wrapMode = GL_MIRRORED_REPEAT; break;
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

        // Set filter mode
        switch (filter)
        {
            case FilterMode::Nearest:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case FilterMode::Linear:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            case FilterMode::Bilinear:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            case FilterMode::Trilinear:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        }

        // Upload texture data
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        
        // Generate mipmaps if using mipmap filtering
        if (filter == FilterMode::Bilinear || filter == FilterMode::Trilinear)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        loaded = true;
        return true;
    }

    /**
     * Create solid color texture (1x1 pixel)
     * @param r Red component (0-1)
     * @param g Green component (0-1)
     * @param b Blue component (0-1)
     * @param a Alpha component (0-1)
     */
    bool createSolidColor(float r, float g, float b, float a = 1.0f)
    {
        unsigned char data[4] = {
            static_cast<unsigned char>(r * 255),
            static_cast<unsigned char>(g * 255),
            static_cast<unsigned char>(b * 255),
            static_cast<unsigned char>(a * 255)
        };
        return createFromData(data, 1, 1, 4, FilterMode::Nearest, WrapMode::Repeat);
    }

    /**
     * Bind texture to a texture unit
     * @param unit Texture unit (0-31)
     */
    void bind(unsigned int unit = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    /**
     * Unbind texture
     */
    void unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint getID() const { return textureID; }
    bool isLoaded() const { return loaded; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getChannels() const { return channels; }
};

#endif //TEXTURE_H
