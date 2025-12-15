//
// Shader System - Compile and manage OpenGL shaders
// Supports custom vertex and fragment shaders with uniform management
//

#ifndef SHADER_H
#define SHADER_H

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "../../Math/vec3.h"
#include "../../Math/mat4.h"
#include "../color.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

/**
 * @class Shader
 * @brief Manages OpenGL shader programs (vertex + fragment)
 * 
 * Handles compilation, linking, and uniform management for GLSL shaders.
 * Supports loading from strings or files, with error reporting.
 */
class Shader
{
private:
    GLuint programID;
    std::unordered_map<std::string, GLint> uniformCache;
    bool compiled;

    /**
     * Compile a single shader (vertex or fragment)
     * @param type GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
     * @param source GLSL source code
     * @return Compiled shader ID, or 0 on error
     */
    GLuint compileShader(GLenum type, const char* source)
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        // Check compilation status
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "ERROR: Shader compilation failed (" 
                      << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") 
                      << ")\n" << infoLog << std::endl;
            return 0;
        }

        return shader;
    }

    /**
     * Get uniform location with caching
     * @param name Uniform variable name in shader
     * @return OpenGL uniform location
     */
    GLint getUniformLocation(const std::string& name)
    {
        if (uniformCache.find(name) != uniformCache.end())
            return uniformCache[name];
        
        GLint location = glGetUniformLocation(programID, name.c_str());
        // Don't warn about missing uniforms - shaders may not use all uniforms
        uniformCache[name] = location;
        return location;
    }

public:
    Shader()
        : programID(0), compiled(false)
    {
    }

    ~Shader()
    {
        if (programID != 0)
        {
            glDeleteProgram(programID);
        }
    }

    /**
     * Compile shader program from source strings
     * @param vertexSource Vertex shader GLSL code
     * @param fragmentSource Fragment shader GLSL code
     * @return true if compilation successful
     */
    bool compileFromSource(const char* vertexSource, const char* fragmentSource)
    {
        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

        if (vertexShader == 0 || fragmentShader == 0)
        {
            if (vertexShader) glDeleteShader(vertexShader);
            if (fragmentShader) glDeleteShader(fragmentShader);
            return false;
        }

        // Link program
        programID = glCreateProgram();
        glAttachShader(programID, vertexShader);
        glAttachShader(programID, fragmentShader);
        glLinkProgram(programID);

        // Check linking status
        GLint success;
        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(programID, 512, nullptr, infoLog);
            std::cerr << "ERROR: Shader program linking failed\n" << infoLog << std::endl;
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return false;
        }

        // Cleanup individual shaders (no longer needed after linking)
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        compiled = true;
        return true;
    }

    /**
     * Load and compile shader from files
     * @param vertexPath Path to vertex shader file
     * @param fragmentPath Path to fragment shader file
     * @return true if compilation successful
     */
    bool compileFromFile(const std::string& vertexPath, const std::string& fragmentPath)
    {
        // Read vertex shader
        std::ifstream vFile(vertexPath);
        if (!vFile.is_open())
        {
            std::cerr << "ERROR: Failed to open vertex shader: " << vertexPath << std::endl;
            return false;
        }
        std::stringstream vStream;
        vStream << vFile.rdbuf();
        std::string vertexCode = vStream.str();

        // Read fragment shader
        std::ifstream fFile(fragmentPath);
        if (!fFile.is_open())
        {
            std::cerr << "ERROR: Failed to open fragment shader: " << fragmentPath << std::endl;
            return false;
        }
        std::stringstream fStream;
        fStream << fFile.rdbuf();
        std::string fragmentCode = fStream.str();

        return compileFromSource(vertexCode.c_str(), fragmentCode.c_str());
    }

    /**
     * Activate this shader for rendering
     */
    void use()
    {
        if (compiled)
        {
            glUseProgram(programID);
        }
    }

    /**
     * Check if shader is compiled and ready
     */
    bool isValid() const { return compiled; }

    /**
     * Get OpenGL program ID
     */
    GLuint getID() const { return programID; }

    // ==================== Uniform Setters ====================
    
    void setInt(const std::string& name, int value)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
            glUniform1i(location, value);
    }

    void setFloat(const std::string& name, float value)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
            glUniform1f(location, value);
    }

    void setBool(const std::string& name, bool value)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
            glUniform1i(location, value ? 1 : 0);
    }

    void setVec3(const std::string& name, const vec3& value)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
            glUniform3f(location, value.x, value.y, value.z);
    }

    void setVec3(const std::string& name, float x, float y, float z)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
            glUniform3f(location, x, y, z);
    }

    void setColor(const std::string& name, const color& value)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
            glUniform3f(location, value.x, value.y, value.z);
    }

    void setMat4(const std::string& name, const mat4& matrix, bool transpose = true)
    {
        GLint location = getUniformLocation(name);
        if (location != -1) {
            // transpose=true because our matrices are row-major, OpenGL expects column-major
            glUniformMatrix4fv(location, 1, transpose ? GL_TRUE : GL_FALSE, &matrix.m[0][0]);
        }
    }
};

#endif //SHADER_H
