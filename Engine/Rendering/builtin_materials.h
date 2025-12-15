//
// Built-in Materials - Unity-like default materials
//

#ifndef BUILTIN_MATERIALS_H
#define BUILTIN_MATERIALS_H

#include "material.h"
#include "Shaders/shader.h"
#include "texture.h"
#include <memory>

/**
 * @class BuiltinMaterials
 * @brief Factory for Unity-like default materials
 * 
 * Provides Standard (PBR), Unlit, and other common materials.
 * Each material type has specific shader and property setup.
 */
class BuiltinMaterials
{
public:
    /**
     * Create Standard (PBR) material
     * Supports albedo, metallic, roughness, normal, and occlusion maps
     * 
     * Properties:
     * - _MainTex: Albedo (diffuse) texture
     * - _Color: Albedo tint color
     * - _MetallicGlossMap: Metallic (R) and Smoothness (A) texture
     * - _Metallic: Metallic factor (0-1)
     * - _Glossiness: Smoothness factor (0-1)
     * - _BumpMap: Normal map texture
     * - _BumpScale: Normal map intensity
     * - _OcclusionMap: Ambient occlusion texture
     * - _OcclusionStrength: AO intensity
     */
    static std::shared_ptr<Material> createStandard()
    {
        const char* vertexShader = R"(
            #version 410 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aNormal;
            layout (location = 2) in vec3 aColor;
            layout (location = 3) in vec2 aTexCoord;
            
            out vec3 FragPos;
            out vec3 Normal;
            out vec3 VertexColor;
            out vec2 TexCoord;
            
            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;
            
            void main()
            {
                FragPos = vec3(model * vec4(aPos, 1.0));
                Normal = mat3(transpose(inverse(model))) * aNormal;
                VertexColor = aColor;
                TexCoord = aTexCoord;
                gl_Position = projection * view * vec4(FragPos, 1.0);
            }
        )";

        const char* fragmentShader = R"(
            #version 410 core
            out vec4 FragColor;
            
            in vec3 FragPos;
            in vec3 Normal;
            in vec3 VertexColor;
            in vec2 TexCoord;
            
            // Material properties
            uniform sampler2D _MainTex;
            uniform vec3 _Color;
            uniform sampler2D _MetallicGlossMap;
            uniform float _Metallic;
            uniform float _Glossiness;
            uniform sampler2D _BumpMap;
            uniform float _BumpScale;
            uniform sampler2D _OcclusionMap;
            uniform float _OcclusionStrength;
            
            uniform bool _UseMainTex;
            uniform bool _UseMetallicMap;
            uniform bool _UseBumpMap;
            uniform bool _UseOcclusionMap;
            
            // Lighting
            uniform vec3 lightDir;
            uniform vec3 lightColor;
            uniform vec3 viewPos;
            uniform vec3 ambientColor;
            
            const float PI = 3.14159265359;
            
            // PBR functions
            float DistributionGGX(vec3 N, vec3 H, float roughness)
            {
                float a = roughness * roughness;
                float a2 = a * a;
                float NdotH = max(dot(N, H), 0.0);
                float NdotH2 = NdotH * NdotH;
                
                float num = a2;
                float denom = (NdotH2 * (a2 - 1.0) + 1.0);
                denom = PI * denom * denom;
                
                return num / denom;
            }
            
            float GeometrySchlickGGX(float NdotV, float roughness)
            {
                float r = (roughness + 1.0);
                float k = (r * r) / 8.0;
                
                float num = NdotV;
                float denom = NdotV * (1.0 - k) + k;
                
                return num / denom;
            }
            
            float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
            {
                float NdotV = max(dot(N, V), 0.0);
                float NdotL = max(dot(N, L), 0.0);
                float ggx2 = GeometrySchlickGGX(NdotV, roughness);
                float ggx1 = GeometrySchlickGGX(NdotL, roughness);
                
                return ggx1 * ggx2;
            }
            
            vec3 fresnelSchlick(float cosTheta, vec3 F0)
            {
                return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
            }
            
            void main()
            {
                // Sample textures
                vec3 albedo = _Color;
                if (_UseMainTex) {
                    albedo *= texture(_MainTex, TexCoord).rgb;
                }
                
                float metallic = _Metallic;
                float smoothness = _Glossiness;
                if (_UseMetallicMap) {
                    vec4 metallicGloss = texture(_MetallicGlossMap, TexCoord);
                    metallic *= metallicGloss.r;
                    smoothness *= metallicGloss.a;
                }
                float roughness = 1.0 - smoothness;
                
                vec3 N = normalize(Normal);
                if (_UseBumpMap) {
                    // Simple normal mapping (proper TBN would be better)
                    vec3 normalMap = texture(_BumpMap, TexCoord).rgb * 2.0 - 1.0;
                    N = normalize(N + normalMap * _BumpScale);
                }
                
                float ao = 1.0;
                if (_UseOcclusionMap) {
                    ao = texture(_OcclusionMap, TexCoord).r;
                    ao = 1.0 - ((1.0 - ao) * _OcclusionStrength);
                }
                
                // PBR lighting
                vec3 V = normalize(viewPos - FragPos);
                vec3 L = normalize(-lightDir);
                vec3 H = normalize(V + L);
                
                // Calculate F0 (reflectance at normal incidence)
                vec3 F0 = vec3(0.04);
                F0 = mix(F0, albedo, metallic);
                
                // Cook-Torrance BRDF
                float NDF = DistributionGGX(N, H, roughness);
                float G = GeometrySmith(N, V, L, roughness);
                vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
                
                vec3 kS = F;
                vec3 kD = vec3(1.0) - kS;
                kD *= 1.0 - metallic;
                
                vec3 numerator = NDF * G * F;
                float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
                vec3 specular = numerator / denominator;
                
                float NdotL = max(dot(N, L), 0.0);
                vec3 Lo = (kD * albedo / PI + specular) * lightColor * NdotL;
                
                // Ambient
                vec3 ambient = ambientColor * albedo * ao;
                
                vec3 finalColor = ambient + Lo;
                
                // Tone mapping and gamma correction
                finalColor = finalColor / (finalColor + vec3(1.0));
                finalColor = pow(finalColor, vec3(1.0/2.2));
                
                FragColor = vec4(finalColor, 1.0);
            }
        )";

        auto shader = std::make_shared<Shader>();
        if (!shader->compileFromSource(vertexShader, fragmentShader))
        {
            std::cerr << "ERROR: Failed to compile Standard material shader" << std::endl;
            return nullptr;
        }

        auto material = std::make_shared<Material>(shader, "Standard");
        
        // Set default properties
        material->setColor("_Color", color(1, 1, 1));
        material->setFloat("_Metallic", 0.0f);
        material->setFloat("_Glossiness", 0.5f);
        material->setFloat("_BumpScale", 1.0f);
        material->setFloat("_OcclusionStrength", 1.0f);
        material->setInt("_UseMainTex", 0);
        material->setInt("_UseMetallicMap", 0);
        material->setInt("_UseBumpMap", 0);
        material->setInt("_UseOcclusionMap", 0);

        return material;
    }

    /**
     * Create Unlit material
     * No lighting calculations, just displays texture/color
     * 
     * Properties:
     * - _MainTex: Main texture
     * - _Color: Tint color
     */
    static std::shared_ptr<Material> createUnlit()
    {
        const char* vertexShader = R"(
            #version 410 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aNormal;
            layout (location = 2) in vec3 aColor;
            layout (location = 3) in vec2 aTexCoord;
            
            out vec3 VertexColor;
            out vec2 TexCoord;
            
            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;
            
            void main()
            {
                VertexColor = aColor;
                TexCoord = aTexCoord;
                gl_Position = projection * view * model * vec4(aPos, 1.0);
            }
        )";

        const char* fragmentShader = R"(
            #version 410 core
            out vec4 FragColor;
            
            in vec3 VertexColor;
            in vec2 TexCoord;
            
            uniform sampler2D _MainTex;
            uniform vec3 _Color;
            uniform bool _UseMainTex;
            
            void main()
            {
                vec3 color = _Color;
                if (_UseMainTex) {
                    color *= texture(_MainTex, TexCoord).rgb;
                }
                FragColor = vec4(color, 1.0);
            }
        )";

        auto shader = std::make_shared<Shader>();
        if (!shader->compileFromSource(vertexShader, fragmentShader))
        {
            std::cerr << "ERROR: Failed to compile Unlit material shader" << std::endl;
            return nullptr;
        }

        auto material = std::make_shared<Material>(shader, "Unlit");
        material->setColor("_Color", color(1, 1, 1));
        material->setInt("_UseMainTex", 0);

        return material;
    }

    /**
     * Create Standard Specular material (non-metallic workflow)
     * Uses specular color instead of metallic parameter
     * 
     * Properties:
     * - _MainTex: Albedo texture
     * - _Color: Albedo tint
     * - _SpecGlossMap: Specular (RGB) and Smoothness (A)
     * - _SpecColor: Specular tint
     * - _Glossiness: Smoothness
     */
    static std::shared_ptr<Material> createStandardSpecular()
    {
        const char* vertexShader = R"(
            #version 410 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aNormal;
            layout (location = 2) in vec3 aColor;
            layout (location = 3) in vec2 aTexCoord;
            
            out vec3 FragPos;
            out vec3 Normal;
            out vec3 VertexColor;
            out vec2 TexCoord;
            
            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;
            
            void main()
            {
                FragPos = vec3(model * vec4(aPos, 1.0));
                Normal = mat3(transpose(inverse(model))) * aNormal;
                VertexColor = aColor;
                TexCoord = aTexCoord;
                gl_Position = projection * view * vec4(FragPos, 1.0);
            }
        )";

        const char* fragmentShader = R"(
            #version 410 core
            out vec4 FragColor;
            
            in vec3 FragPos;
            in vec3 Normal;
            in vec3 VertexColor;
            in vec2 TexCoord;
            
            uniform sampler2D _MainTex;
            uniform vec3 _Color;
            uniform sampler2D _SpecGlossMap;
            uniform vec3 _SpecColor;
            uniform float _Glossiness;
            uniform bool _UseMainTex;
            uniform bool _UseSpecGlossMap;
            
            uniform vec3 lightDir;
            uniform vec3 lightColor;
            uniform vec3 viewPos;
            uniform vec3 ambientColor;
            
            void main()
            {
                vec3 albedo = _Color;
                if (_UseMainTex) {
                    albedo *= texture(_MainTex, TexCoord).rgb;
                }
                
                vec3 specular = _SpecColor;
                float smoothness = _Glossiness;
                if (_UseSpecGlossMap) {
                    vec4 specGloss = texture(_SpecGlossMap, TexCoord);
                    specular *= specGloss.rgb;
                    smoothness *= specGloss.a;
                }
                
                vec3 N = normalize(Normal);
                vec3 L = normalize(-lightDir);
                vec3 V = normalize(viewPos - FragPos);
                vec3 H = normalize(L + V);
                
                // Diffuse
                float diff = max(dot(N, L), 0.0);
                vec3 diffuse = diff * lightColor * albedo;
                
                // Specular (Blinn-Phong)
                float spec = pow(max(dot(N, H), 0.0), smoothness * 128.0);
                vec3 specularColor = spec * lightColor * specular;
                
                // Ambient
                vec3 ambient = ambientColor * albedo;
                
                vec3 finalColor = ambient + diffuse + specularColor;
                
                // Gamma correction
                finalColor = pow(finalColor, vec3(1.0/2.2));
                
                FragColor = vec4(finalColor, 1.0);
            }
        )";

        auto shader = std::make_shared<Shader>();
        if (!shader->compileFromSource(vertexShader, fragmentShader))
        {
            std::cerr << "ERROR: Failed to compile Standard Specular material shader" << std::endl;
            return nullptr;
        }

        auto material = std::make_shared<Material>(shader, "Standard (Specular)");
        material->setColor("_Color", color(1, 1, 1));
        material->setColor("_SpecColor", color(0.2f, 0.2f, 0.2f));
        material->setFloat("_Glossiness", 0.5f);
        material->setInt("_UseMainTex", 0);
        material->setInt("_UseSpecGlossMap", 0);

        return material;
    }
};

#endif //BUILTIN_MATERIALS_H
