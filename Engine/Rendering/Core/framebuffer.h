#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "../color.h"
#include <vector>
#include <limits>
#include <algorithm>
#include <fstream>
#include <iostream>

/**
 * @class Framebuffer
 * @brief Simple framebuffer for software rendering
 * 
 * Manages a color buffer and depth buffer for rendering.
 * Supports clearing, setting pixels, and saving to PPM image files.
 * Used by the Rasterizer for offscreen rendering.
 */
class Framebuffer
{
public:
    int width;
    int height;
    std::vector<color> colorBuffer;
    std::vector<float> depthBuffer;

    /**
     * @brief Construct a new Framebuffer object
     * @param w Width of the framebuffer
     * @param h Height of the framebuffer
     */
    Framebuffer(int w, int h)
        : width(w), height(h)
    {
        colorBuffer.resize(width * height, color(0, 0, 0));
        depthBuffer.resize(width * height, 1.0f); // Use 1.0 for far plane (depth range [0,1])
    }

    /**
     * @brief Clear the framebuffer with a specified color
     * @param clearColor Color to clear the framebuffer with
     */
    void clear(const color& clearColor = color(0.1f, 0.1f, 0.15f))
    {
        std::fill(colorBuffer.begin(), colorBuffer.end(), clearColor);
        std::fill(depthBuffer.begin(), depthBuffer.end(), 1.0f); // Clear to far plane
    }

    /**
     * @brief Set a pixel color at specified coordinates
     * @param x X coordinate
     * @param y Y coordinate
     * @param col Color to set
     */
    void setPixel(int x, int y, const color& col)
    {
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            colorBuffer[y * width + x] = col;
        }
    }

    /**
     * @brief Set a pixel color with depth testing
     * @param x X coordinate
     * @param y Y coordinate
     * @param depth Depth value (0 = near, 1 = far)
     * @param col Color to set
     */
    void setPixelWithDepth(int x, int y, float depth, const color& col)
    {
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            int index = y * width + x;
            if (depth < depthBuffer[index])
            {
                depthBuffer[index] = depth;
                colorBuffer[index] = col;
            }
        }
    }

    /**
     * @brief Get the color of a pixel at specified coordinates
     * @param x X coordinate
     * @param y Y coordinate
     * @return Color at the specified pixel
     */
    color getPixel(int x, int y) const
    {
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            return colorBuffer[y * width + x];
        }
        return color(0, 0, 0);
    }

    /**
     * @brief Get the depth value at specified coordinates
     * @param x X coordinate
     * @param y Y coordinate
     * @return Depth value at the specified pixel
     */
    float getDepth(int x, int y) const
    {
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            return depthBuffer[y * width + x];
        }
        return 1.0f; // Return far plane for out of bounds
    }

    /**
     * @brief Save framebuffer as PPM image
     * @param filename Output file name
     */
    void saveToPPM(const std::string& filename) const
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }

        file << "P3\n" << width << " " << height << "\n255\n";

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                color c = getPixel(x, y);
                write_color(file, c);
            }
        }

        file.close();
    }

    /**
     * @brief Output framebuffer to console in PPM format
     */
    void outputToConsole() const
    {
        std::cout << "P3\n" << width << " " << height << "\n255\n";

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                color c = getPixel(x, y);
                write_color(std::cout, c);
            }
        }
    }

    /**
     * @brief Resize the framebuffer
     * @param newWidth New width
     * @param newHeight New height
     */
    void resize(int newWidth, int newHeight)
    {
        width = newWidth;
        height = newHeight;
        colorBuffer.resize(width * height);
        depthBuffer.resize(width * height);
        clear();
    }

    /**
     * @brief Get raw pixel data as a byte array (RGB format)
     * @return Vector of unsigned char containing pixel data
     */
    std::vector<unsigned char> getPixelData() const
    {
        std::vector<unsigned char> pixels(width * height * 3);
        
        for (int i = 0; i < width * height; i++)
        {
            const color& c = colorBuffer[i];
            pixels[i * 3 + 0] = static_cast<unsigned char>(std::min(255.0f, std::max(0.0f, c.x * 255.999f)));
            pixels[i * 3 + 1] = static_cast<unsigned char>(std::min(255.0f, std::max(0.0f, c.y * 255.999f)));
            pixels[i * 3 + 2] = static_cast<unsigned char>(std::min(255.0f, std::max(0.0f, c.z * 255.999f)));
        }
        
        return pixels;
    }
};

#endif //FRAMEBUFFER_H
