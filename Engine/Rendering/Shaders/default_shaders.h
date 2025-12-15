//
// Default Shaders - Built-in GLSL shaders for the engine
// These can be used as templates for custom shaders
//

#ifndef DEFAULT_SHADERS_H
#define DEFAULT_SHADERS_H

namespace DefaultShaders
{
    /**
     * Basic Blinn-Phong Vertex Shader
     * Transforms vertices and passes data to fragment shader
     */
    const char* BLINN_PHONG_VERTEX = R"(
#version 330 core

// Input vertex attributes
layout (location = 0) in vec3 aPos;       // Vertex position
layout (location = 1) in vec3 aNormal;    // Vertex normal
layout (location = 2) in vec3 aColor;     // Vertex color

// Output to fragment shader
out vec3 FragPos;      // World-space position
out vec3 Normal;       // World-space normal
out vec3 VertexColor;  // Interpolated vertex color

// Transformation matrices
uniform mat4 model;       // Model matrix (local to world)
uniform mat4 view;        // View matrix (world to camera)
uniform mat4 projection;  // Projection matrix (camera to clip space)

void main()
{
    // Transform position to world space
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Transform normal to world space (using normal matrix)
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Pass through vertex color
    VertexColor = aColor;
    
    // Final position in clip space
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

    /**
     * Blinn-Phong Fragment Shader
     * Implements ambient + diffuse + specular lighting
     * Supports up to 8 lights (directional or point)
     */
    const char* BLINN_PHONG_FRAGMENT = R"(
#version 330 core

// Input from vertex shader
in vec3 FragPos;      // World-space fragment position
in vec3 Normal;       // World-space normal
in vec3 VertexColor;  // Interpolated vertex color

// Output color
out vec4 FragColor;

// Light structure
struct Light {
    int type;         // 0 = directional, 1 = point
    vec3 position;    // Position (for point lights)
    vec3 direction;   // Direction (for directional lights)
    vec3 color;       // Light color
    float intensity;  // Light intensity multiplier
};

// Uniforms
uniform vec3 viewPos;           // Camera position
uniform Light lights[8];        // Array of lights
uniform int numLights;          // Number of active lights

// Material properties (could be made uniform for customization)
const float ambientStrength = 0.1;
const float specularStrength = 0.5;
const float shininess = 32.0;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Ambient lighting (constant base light)
    vec3 ambient = ambientStrength * VertexColor;
    
    // Accumulate diffuse and specular from all lights
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    
    for (int i = 0; i < numLights && i < 8; i++)
    {
        vec3 lightDir;
        float attenuation = 1.0;
        
        if (lights[i].type == 0) // Directional light
        {
            lightDir = normalize(-lights[i].direction);
        }
        else // Point light
        {
            vec3 toLight = lights[i].position - FragPos;
            float distance = length(toLight);
            lightDir = normalize(toLight);
            
            // Attenuation: 1 / (constant + linear*d + quadratic*d^2)
            attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        }
        
        // Diffuse lighting (Lambert's cosine law)
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += lights[i].color * lights[i].intensity * diff * attenuation * VertexColor;
        
        // Specular lighting (Blinn-Phong)
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfDir), 0.0), shininess);
        specular += lights[i].color * lights[i].intensity * spec * attenuation * specularStrength;
    }
    
    // Combine all lighting components
    vec3 result = ambient + diffuse + specular;
    
    // Clamp to valid color range [0, 1]
    result = clamp(result, 0.0, 1.0);
    
    FragColor = vec4(result, 1.0);
}
)";

    /**
     * Unlit Vertex Shader
     * Simple vertex transformation without lighting
     */
    const char* UNLIT_VERTEX = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aColor;

out vec3 VertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    VertexColor = aColor;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

    /**
     * Unlit Fragment Shader
     * Outputs vertex color directly without lighting
     */
    const char* UNLIT_FRAGMENT = R"(
#version 330 core
in vec3 VertexColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(VertexColor, 1.0);
}
)";

    /**
     * Wireframe Fragment Shader
     * Solid color output (useful for debug rendering)
     */
    const char* WIREFRAME_FRAGMENT = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 wireframeColor;

void main()
{
    FragColor = vec4(wireframeColor, 1.0);
}
)";
}

#endif //DEFAULT_SHADERS_H
