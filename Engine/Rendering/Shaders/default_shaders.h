//
// Default Shaders - Built-in GLSL shaders for the engine
// These can be used as templates for custom shaders
//

#ifndef DEFAULT_SHADERS_H
#define DEFAULT_SHADERS_H

namespace DefaultShaders
{
    /**
     * Optimized Blinn-Phong Vertex Shader with UBOs
     * Supports packed vertex format and uniform buffers
     */
    const char* BLINN_PHONG_VERTEX = R"(
#version 330 core

// Input vertex attributes (packed format)
layout (location = 0) in vec3 aPos;        // Position (vec3)
layout (location = 1) in vec2 aNormalPacked; // Packed normal (octahedron)
layout (location = 2) in vec2 aTexCoord;   // UV (half float)
layout (location = 3) in vec4 aColor;      // Color (4x uint8 normalized)

// Camera UBO (shared across all draws)
layout (std140) uniform CameraData
{
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    vec3 cameraPosition;
};

// Per-object uniforms (still needed)
uniform mat4 model;

// Output to fragment shader
out vec3 FragPos;      // World-space position
out vec3 Normal;       // World-space normal
out vec2 TexCoord;     // Texture coordinates
out vec4 VertexColor;  // Interpolated vertex color

// Unpack octahedron normal
vec3 unpackNormal(vec2 packed)
{
    vec3 n;
    n.z = 1.0 - abs(packed.x) - abs(packed.y);
    
    if (n.z < 0.0)
    {
        float signX = packed.x >= 0.0 ? 1.0 : -1.0;
        float signY = packed.y >= 0.0 ? 1.0 : -1.0;
        float oldX = packed.x;
        n.x = (1.0 - abs(packed.y)) * signX;
        n.y = (1.0 - abs(oldX)) * signY;
    }
    else
    {
        n.x = packed.x;
        n.y = packed.y;
    }
    
    return normalize(n);
}

void main()
{
    // Transform position to world space
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Unpack and transform normal to world space
    vec3 localNormal = unpackNormal(aNormalPacked);
    Normal = mat3(transpose(inverse(model))) * localNormal;
    
    // Pass through texture coordinates and color
    TexCoord = aTexCoord;
    VertexColor = aColor;
    
    // Final position in clip space (using UBO)
    gl_Position = viewProjection * vec4(FragPos, 1.0);
}
)";

    /**
     * Optimized Blinn-Phong Fragment Shader with UBOs
     * Uses uniform buffers for light data
     */
    const char* BLINN_PHONG_FRAGMENT = R"(
#version 330 core

// Input from vertex shader
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 VertexColor;

// Output color
out vec4 FragColor;

// Light structure (std140 layout)
struct Light {
    vec3 position;
    int type;
    vec3 direction;
    float intensity;
    vec3 color;
    float _pad0;
};

// Lights UBO (shared across all draws)
layout (std140) uniform LightData
{
    Light lights[8];
    int numLights;
};

// Camera UBO (for view position)
layout (std140) uniform CameraData
{
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    vec3 cameraPosition;
};

// Material properties (could be made uniform for customization)
const float ambientStrength = 0.1;
const float specularStrength = 0.5;
const float shininess = 32.0;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(cameraPosition - FragPos);
    
    // Ambient lighting
    vec3 ambient = ambientStrength * VertexColor.rgb;
    
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
        diffuse += lights[i].color * lights[i].intensity * diff * attenuation * VertexColor.rgb;
        
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
     * Unlit Vertex Shader (optimized with UBOs)
     * Simple vertex transformation without lighting
     */
    const char* UNLIT_VERTEX = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aNormalPacked;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec4 aColor;

out vec4 VertexColor;

// Camera UBO
layout (std140) uniform CameraData
{
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    vec3 cameraPosition;
};

uniform mat4 model;

void main()
{
    VertexColor = aColor;
    gl_Position = viewProjection * model * vec4(aPos, 1.0);
}
)";

    /**
     * Unlit Fragment Shader
     * Outputs vertex color directly without lighting
     */
    const char* UNLIT_FRAGMENT = R"(
#version 330 core
in vec4 VertexColor;
out vec4 FragColor;

void main()
{
    FragColor = VertexColor;
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
