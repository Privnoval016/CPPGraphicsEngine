//
// Created by Graphics Engine
//

#ifndef LIGHT_H
#define LIGHT_H

#include "../Math/vec3.h"
#include "color.h"

class Light
{
public:
    enum class Type
    {
        Directional,
        Point,
        Spot
    };

    Type type;
    vec3 position;      // For point and spot lights
    vec3 direction;     // For directional and spot lights
    color color;
    float intensity;
    float range;        // For point lights
    float spotAngle;    // For spot lights (in radians)

    // Directional light (like the sun)
    static Light directional(const vec3& dir, const ::color& col = ::color(1, 1, 1), float intensity = 1.0f)
    {
        Light light;
        light.type = Type::Directional;
        light.direction = normalize(dir);
        light.color = col;
        light.intensity = intensity;
        light.position = vec3::zero;
        light.range = 0;
        light.spotAngle = 0;
        return light;
    }

    // Point light (radiates in all directions)
    static Light point(const vec3& pos, const ::color& col = ::color(1, 1, 1), float intensity = 1.0f, float range = 10.0f)
    {
        Light light;
        light.type = Type::Point;
        light.position = pos;
        light.color = col;
        light.intensity = intensity;
        light.range = range;
        light.direction = vec3::zero;
        light.spotAngle = 0;
        return light;
    }

    // Spot light (cone of light)
    static Light spot(const vec3& pos, const vec3& dir, const ::color& col = ::color(1, 1, 1), 
                      float intensity = 1.0f, float angle = 45.0f, float range = 10.0f)
    {
        Light light;
        light.type = Type::Spot;
        light.position = pos;
        light.direction = normalize(dir);
        light.color = col;
        light.intensity = intensity;
        light.range = range;
        light.spotAngle = angle * 3.14159f / 180.0f;
        return light;
    }

    Light()
        : type(Type::Directional),
          position(vec3::zero),
          direction(0, -1, 0),
          color(1, 1, 1),
          intensity(1.0f),
          range(10.0f),
          spotAngle(45.0f * 3.14159f / 180.0f)
    {
    }
};

#endif //LIGHT_H
