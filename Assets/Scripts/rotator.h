//
// Example User Scripts
//

#ifndef ROTATOR_H
#define ROTATOR_H

#include "../../Engine/Core/Components/monoBehaviour.h"
#include "../../Engine/Math/vec3.h"
#include <cmath>

// Example MonoBehaviour that rotates an object
class Rotator : public MonoBehaviour
{
public:
    vec3 rotationSpeed; // Rotation speed in radians per second

    Rotator() : rotationSpeed(0, 1.0f, 0) {}

    Rotator(const vec3& speed) : rotationSpeed(speed) {}

    void start() override
    {
        // Called when the component starts
    }

    void update(float deltaTime) override
    {
        // Rotate the object
        transform().rotate(rotationSpeed * deltaTime);
    }
};

// Another example: Oscillator that moves object up and down
class Oscillator : public MonoBehaviour
{
public:
    float amplitude;
    float frequency;
    vec3 axis;
    vec3 startPosition;

private:
    float timeAccum;

public:
    Oscillator()
        : amplitude(1.0f),
          frequency(1.0f),
          axis(0, 1, 0),
          timeAccum(0.0f)
    {
    }

    void start() override
    {
        startPosition = transform().getWorldPosition();
    }

    void update(float deltaTime) override
    {
        timeAccum += deltaTime;
        float offset = std::sin(timeAccum * frequency * 2.0f * 3.14159f) * amplitude;
        transform().setWorldPosition(startPosition + axis * offset);
    }
};

// Example: Orbiter that orbits around a point
class Orbiter : public MonoBehaviour
{
public:
    vec3 center;
    float radius;
    float speed;

private:
    float angle;

public:
    Orbiter()
        : center(vec3::zero),
          radius(5.0f),
          speed(1.0f),
          angle(0.0f)
    {
    }

    void update(float deltaTime) override
    {
        angle += speed * deltaTime;
        
        float x = center.x + radius * std::cos(angle);
        float z = center.z + radius * std::sin(angle);
        
        transform().setWorldPosition(vec3(x, center.y, z));
    }
};

#endif //ROTATOR_H
