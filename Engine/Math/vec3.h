//
// Created by Pranav Sukesh on 8/27/25.
//

#ifndef VEC3_H
#define VEC3_H

#include <iostream>
#include <cmath>


class vec3
{
public:
    static const vec3 zero;
    static const vec3 one;
    static const vec3 up;
    static const vec3 forward;
    static const vec3 right;

    float e[3];

    vec3() : e{0,0,0} {}
    vec3(const float e0, const float e1, const float e2) : e{e0, e1, e2} {}

    [[nodiscard]] float x() const { return e[0]; }
    [[nodiscard]] float y() const { return e[1]; }
    [[nodiscard]] float z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    float operator[](int i) const { return e[i]; }
    float& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3 &v)
    {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator-=(const vec3 &v)
    {
        e[0] -= v.e[0];
        e[1] -= v.e[1];
        e[2] -= v.e[2];
        return *this;
    }

    vec3& operator*=(const float t)
    {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(const float t)
    {
        return *this *= 1/t;
    }

    [[nodiscard]] float magnitude() const
    {
        return sqrt(sqr_magnitude());
    }

    [[nodiscard]] float sqr_magnitude() const
    {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }
};

const vec3 vec3::zero = vec3(0, 0, 0);
const vec3 vec3::one = vec3(1, 1, 1);
const vec3 vec3::up = vec3(0, 1, 0);
const vec3 vec3::forward = vec3(0, 0, 1);
const vec3 vec3::right = vec3(1, 0, 0);


inline std::ostream& operator<<(std::ostream &out, const vec3 &v)
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v)
{
    return {u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]};
}

inline vec3 operator-(const vec3 &u, const vec3 &v)
{
    return {u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]};
}

inline vec3 operator*(const vec3 &u, const vec3 &v)
{
    return {u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]};
}

inline vec3 operator*(const float t, const vec3 &v)
{
    return {t*v.e[0], t*v.e[1], t*v.e[2]};
}

inline vec3 operator*(const vec3 &v, const float t)
{
    return t * v;
}

inline vec3 operator/(const vec3 &v, const float t)
{
    return (1/t) * v;
}

inline float dot(const vec3 &u, const vec3 &v)
{
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v)
{
    return {u.e[1] * v.e[2] - u.e[2] * v.e[1],
            u.e[2] * v.e[0] - u.e[0] * v.e[2],
            u.e[0] * v.e[1] - u.e[1] * v.e[0]};
}

inline vec3 normalize(const vec3 &v)
{
    return v / v.magnitude();
}

inline vec3 reflect(const vec3 &v, const vec3 &n)
{
    return v - 2.0f * dot(v, n) * n;
}

inline vec3 lerp(const vec3 &a, const vec3 &b, float t)
{
    return a + t * (b - a);
}


#endif //VEC3_H
