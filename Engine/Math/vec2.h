#ifndef VEC2_H
#define VEC2_H

#include <cmath>

/**
 * @struct vec2
 * @brief 2D vector for texture coordinates, screen positions, etc.
 */
struct vec2
{
    float x, y;

    // Constructors
    vec2() : x(0), y(0) {}
    vec2(float x, float y) : x(x), y(y) {}
    vec2(float value) : x(value), y(value) {}

    // Basic operations
    vec2 operator+(const vec2& other) const { return vec2(x + other.x, y + other.y); }
    vec2 operator-(const vec2& other) const { return vec2(x - other.x, y - other.y); }
    vec2 operator*(float scalar) const { return vec2(x * scalar, y * scalar); }
    vec2 operator/(float scalar) const { return vec2(x / scalar, y / scalar); }

    vec2& operator+=(const vec2& other) { x += other.x; y += other.y; return *this; }
    vec2& operator-=(const vec2& other) { x -= other.x; y -= other.y; return *this; }
    vec2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    vec2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

    // Comparison
    bool operator==(const vec2& other) const { return x == other.x && y == other.y; }
    bool operator!=(const vec2& other) const { return !(*this == other); }

    // Utility
    float length() const { return std::sqrt(x * x + y * y); }
    float lengthSquared() const { return x * x + y * y; }
    
    vec2 normalized() const {
        float len = length();
        return len > 0 ? vec2(x / len, y / len) : vec2(0, 0);
    }

    static float dot(const vec2& a, const vec2& b) {
        return a.x * b.x + a.y * b.y;
    }

    static float distance(const vec2& a, const vec2& b) {
        return (b - a).length();
    }

    // Common vectors
    static const vec2 zero;
    static const vec2 one;
    static const vec2 up;
    static const vec2 down;
    static const vec2 left;
    static const vec2 right;
};

// Static member definitions
inline const vec2 vec2::zero = vec2(0, 0);
inline const vec2 vec2::one = vec2(1, 1);
inline const vec2 vec2::up = vec2(0, 1);
inline const vec2 vec2::down = vec2(0, -1);
inline const vec2 vec2::left = vec2(-1, 0);
inline const vec2 vec2::right = vec2(1, 0);

// Scalar * vec2
inline vec2 operator*(float scalar, const vec2& v) {
    return v * scalar;
}

#endif
