# Material System Guide

## Overview

The material system provides Unity-like functionality for creating, configuring, and saving materials with PBR textures. Materials can be created programmatically and saved to `.mat` files for reuse.

## Creating Materials from Textures

### Basic PBR Material

```cpp
// Create a Standard (PBR) material
auto material = BuiltinMaterials::createStandard();
material->setName("MyMaterial");

// Load and set textures with file paths
std::string diffusePath = "Assets/Textures/diffuse.png";
auto diffuseTex = TextureLoader::loadFromFile(diffusePath);
if (diffuseTex) {
    material->setTexture("_MainTex", diffuseTex, diffusePath);
    material->setInt("_UseMainTex", 1);  // Enable texture (like Unity's checkbox)
}

// Set material properties
material->setColor("_Color", color(1.0f, 1.0f, 1.0f));
material->setFloat("_Metallic", 0.5f);
material->setFloat("_Smoothness", 0.7f);

// Save material to file
MaterialSerializer::saveToFile(*material, "Assets/Materials/my_material.mat");
```

**Note**: Like Unity, you must set the `_Use*` flags to 1 to enable textures. This is equivalent to Unity's checkboxes next to texture slots.

### Full PBR Material with All Maps

```cpp
auto material = BuiltinMaterials::createStandard();
material->setName("DetailedMaterial");

// Albedo (diffuse) texture
std::string albedoPath = "Assets/Textures/albedo.png";
auto albedo = TextureLoader::loadFromFile(albedoPath);
if (albedo) {
    material->setTexture("_MainTex", albedo, albedoPath);
    material->setInt("_UseMainTex", 1);
}

// Metallic/Smoothness map (R = metallic, A = smoothness)
std::string metallicPath = "Assets/Textures/metallic.png";
auto metallic = TextureLoader::loadFromFile(metallicPath);
if (metallic) {
    material->setTexture("_MetallicGlossMap", metallic, metallicPath);
    material->setInt("_UseMetallicMap", 1);
}

// Normal map
std::string normalPath = "Assets/Textures/normal.png";
auto normal = TextureLoader::loadFromFile(normalPath);
if (normal) {
    material->setTexture("_BumpMap", normal, normalPath);
    material->setFloat("_BumpScale", 1.0f);
    material->setInt("_UseBumpMap", 1);
}

// Occlusion map
std::string aoPath = "Assets/Textures/ao.png";
auto ao = TextureLoader::loadFromFile(aoPath);
if (ao) {
    material->setTexture("_OcclusionMap", ao, aoPath);
    material->setFloat("_OcclusionStrength", 1.0f);
    material->setInt("_UseOcclusionMap", 1);
}

// Set material properties
material->setColor("_Color", color(1.0f, 1.0f, 1.0f));
material->setFloat("_Metallic", 0.8f);
material->setFloat("_Smoothness", 0.9f);

// Save to file
MaterialSerializer::saveToFile(*material, "Assets/Materials/detailed.mat");
```

## Loading Materials from Files

```cpp
// Load a material from a .mat file
auto material = MaterialSerializer::loadFromFile("Assets/Materials/my_material.mat");

if (material) {
    // Apply to mesh renderer
    meshRenderer->setMaterial(material);
}
```

## Material File Format

Material files (`.mat`) are human-readable text files:

```
materialType: Standard

# Color Properties
color: _Color 1.0 1.0 1.0

# Float Properties
float: _Metallic 0.5
float: _Smoothness 0.7
float: _BumpScale 1.0

# Texture Properties
texture: _MainTex Assets/Textures/diffuse.png
texture: _MetallicGlossMap Assets/Textures/metallic.png
texture: _BumpMap Assets/Textures/normal.png
```

## Standard Material Properties

### Texture Properties
- `_MainTex` - Albedo (diffuse) texture
- `_MetallicGlossMap` - Metallic (R channel) and Smoothness (A channel)
- `_BumpMap` - Normal map
- `_OcclusionMap` - Ambient occlusion map

### Texture Enable Flags (Int Properties)
Like Unity's checkboxes, these must be set to 1 to enable texture usage:
- `_UseMainTex` - Enable albedo texture (0 = use _Color only, 1 = use texture)
- `_UseMetallicMap` - Enable metallic/smoothness texture
- `_UseBumpMap` - Enable normal map
- `_UseOcclusionMap` - Enable ambient occlusion texture

### Color Properties
- `_Color` - Albedo tint color (multiplied with texture)

### Float Properties
- `_Metallic` - Metallic factor (0 = dielectric, 1 = metal)
- `_Smoothness` - Smoothness factor (0 = rough, 1 = smooth)
- `_BumpScale` - Normal map intensity (0-2)
- `_OcclusionStrength` - AO intensity (0-1)

## Built-in Material Types

- `Standard` - PBR material with full lighting
- `StandardSpecular` - PBR with specular workflow
- `Unlit` - No lighting, just displays texture/color

## Custom Material Types

Register custom material types for serialization:

```cpp
MaterialSerializer::registerMaterialType("CustomPBR", []() {
    auto mat = std::make_shared<Material>(customShader, "CustomPBR");
    // Set up default properties
    return mat;
});
```

## Example: Ground Material

See `Demos/asset_demo.cpp` for a complete example of creating a ground material with PBR textures.
