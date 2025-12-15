#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

/**
 * @struct vec3
 * @brief 3D vector for positions, directions, colors, etc.
 */
struct vec3
{
    float x, y, z;

    // Constructors
    vec3() : x(0), y(0), z(0) {}
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    vec3(float value) : x(value), y(value), z(value) {}

    // Array-style access for compatibility
    float operator[](int i) const { return i == 0 ? x : (i == 1 ? y : z); }
    float& operator[](int i) { return i == 0 ? x : (i == 1 ? y : z); }

    // Basic operations
    vec3 operator-() const { return vec3(-x, -y, -z); }
    vec3 operator+(const vec3& other) const { return vec3(x + other.x, y + other.y, z + other.z); }
    vec3 operator-(const vec3& other) const { return vec3(x - other.x, y - other.y, z - other.z); }
    vec3 operator*(const vec3& other) const { return vec3(x * other.x, y * other.y, z * other.z); }
    vec3 operator*(float scalar) const { return vec3(x * scalar, y * scalar, z * scalar); }
    vec3 operator/(float scalar) const { return vec3(x / scalar, y / scalar, z / scalar); }

    vec3& operator+=(const vec3& other) { x += other.x; y += other.y; z += other.z; return *this; }
    vec3& operator-=(const vec3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    vec3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    vec3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

    // Comparison
    bool operator==(const vec3& other) const { return x == other.x && y == other.y && z == other.z; }
    bool operator!=(const vec3& other) const { return !(*this == other); }

    // Utility
    float length() const { return std::sqrt(x * x + y * y + z * z); }
    float lengthSquared() const { return x * x + y * y + z * z; }
    
    vec3 normalized() const {
        float len = length();
        return len > 0 ? vec3(x / len, y / len, z / len) : vec3(0, 0, 0);
    }

    // Static utility functions
    static float dot(const vec3& a, const vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static vec3 cross(const vec3& a, const vec3& b) {
        return vec3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    static float distance(const vec3& a, const vec3& b) {
        return (b - a).length();
    }

    static vec3 reflect(const vec3& v, const vec3& n) {
        return v - n * (2.0f * dot(v, n));
    }

    static vec3 lerp(const vec3& a, const vec3& b, float t) {
        return a + (b - a) * t;
    }

    // Common vectors
    static const vec3 zero;
    static const vec3 one;
    static const vec3 up;
    static const vec3 down;
    static const vec3 forward;
    static const vec3 back;
    static const vec3 right;
    static const vec3 left;
};

// Static member definitions
inline const vec3 vec3::zero = vec3(0, 0, 0);
inline const vec3 vec3::one = vec3(1, 1, 1);
inline const vec3 vec3::up = vec3(0, 1, 0);
inline const vec3 vec3::down = vec3(0, -1, 0);
inline const vec3 vec3::forward = vec3(0, 0, 1);
inline const vec3 vec3::back = vec3(0, 0, -1);
inline const vec3 vec3::right = vec3(1, 0, 0);
inline const vec3 vec3::left = vec3(-1, 0, 0);

// Scalar * vec3
inline vec3 operator*(float scalar, const vec3& v) {
    return v * scalar;
}

// Stream output
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.x << ' ' << v.y << ' ' << v.z;
}

#endif //VEC3_H
