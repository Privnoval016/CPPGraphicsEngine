//
// Created by Graphics Engine
//

#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"
#include <cmath>
#include <iostream>

// Forward declaration
class vec4;

class mat4
{
public:
    float m[4][4];

    // Constructors
    mat4()
    {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    explicit mat4(float diagonal)
    {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = (i == j) ? diagonal : 0.0f;
    }

    mat4(float m00, float m01, float m02, float m03,
         float m10, float m11, float m12, float m13,
         float m20, float m21, float m22, float m23,
         float m30, float m31, float m32, float m33)
    {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
        m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }

    // Static factory methods
    static mat4 identity()
    {
        return mat4();
    }

    static mat4 translation(const vec3& pos)
    {
        mat4 result;
        result.m[0][3] = pos.x();
        result.m[1][3] = pos.y();
        result.m[2][3] = pos.z();
        return result;
    }

    static mat4 scale(const vec3& s)
    {
        mat4 result;
        result.m[0][0] = s.x();
        result.m[1][1] = s.y();
        result.m[2][2] = s.z();
        return result;
    }

    static mat4 rotationX(float angle)
    {
        mat4 result;
        float c = std::cos(angle);
        float s = std::sin(angle);
        result.m[1][1] = c;
        result.m[1][2] = -s;
        result.m[2][1] = s;
        result.m[2][2] = c;
        return result;
    }

    static mat4 rotationY(float angle)
    {
        mat4 result;
        float c = std::cos(angle);
        float s = std::sin(angle);
        result.m[0][0] = c;
        result.m[0][2] = s;
        result.m[2][0] = -s;
        result.m[2][2] = c;
        return result;
    }

    static mat4 rotationZ(float angle)
    {
        mat4 result;
        float c = std::cos(angle);
        float s = std::sin(angle);
        result.m[0][0] = c;
        result.m[0][1] = -s;
        result.m[1][0] = s;
        result.m[1][1] = c;
        return result;
    }

    static mat4 euler(const vec3& angles)
    {
        return rotationZ(angles.z()) * rotationY(angles.y()) * rotationX(angles.x());
    }

    static mat4 axisAngle(const vec3& axis, float angle)
    {
        mat4 result;
        vec3 a = normalize(axis);
        float c = std::cos(angle);
        float s = std::sin(angle);
        float t = 1.0f - c;
        
        result.m[0][0] = t * a.x() * a.x() + c;
        result.m[0][1] = t * a.x() * a.y() - s * a.z();
        result.m[0][2] = t * a.x() * a.z() + s * a.y();
        
        result.m[1][0] = t * a.x() * a.y() + s * a.z();
        result.m[1][1] = t * a.y() * a.y() + c;
        result.m[1][2] = t * a.y() * a.z() - s * a.x();
        
        result.m[2][0] = t * a.x() * a.z() - s * a.y();
        result.m[2][1] = t * a.y() * a.z() + s * a.x();
        result.m[2][2] = t * a.z() * a.z() + c;
        
        return result;
    }

    static mat4 perspective(float fov, float aspect, float near, float far)
    {
        mat4 result(0.0f);
        float tanHalfFov = std::tan(fov / 2.0f);
        
        result.m[0][0] = 1.0f / (aspect * tanHalfFov);
        result.m[1][1] = 1.0f / tanHalfFov;
        result.m[2][2] = -(far + near) / (far - near);
        result.m[2][3] = -(2.0f * far * near) / (far - near);
        result.m[3][2] = -1.0f;
        result.m[3][3] = 0.0f;
        
        return result;
    }

    static mat4 orthographic(float left, float right, float bottom, float top, float near, float far)
    {
        mat4 result;
        result.m[0][0] = 2.0f / (right - left);
        result.m[1][1] = 2.0f / (top - bottom);
        result.m[2][2] = -2.0f / (far - near);
        result.m[0][3] = -(right + left) / (right - left);
        result.m[1][3] = -(top + bottom) / (top - bottom);
        result.m[2][3] = -(far + near) / (far - near);
        return result;
    }

    static mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up)
    {
        vec3 f = normalize(center - eye);
        vec3 s = normalize(cross(f, up));
        vec3 u = cross(s, f);

        mat4 result;
        result.m[0][0] = s.x();
        result.m[0][1] = s.y();
        result.m[0][2] = s.z();
        result.m[1][0] = u.x();
        result.m[1][1] = u.y();
        result.m[1][2] = u.z();
        result.m[2][0] = -f.x();
        result.m[2][1] = -f.y();
        result.m[2][2] = -f.z();
        result.m[0][3] = -dot(s, eye);
        result.m[1][3] = -dot(u, eye);
        result.m[2][3] = dot(f, eye);
        
        return result;
    }

    // Operators
    mat4 operator*(const mat4& other) const
    {
        mat4 result;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 4; k++)
                    result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
        return result;
    }

    mat4& operator*=(const mat4& other)
    {
        *this = *this * other;
        return *this;
    }

    vec3 transformPoint(const vec3& v) const
    {
        float x = m[0][0] * v.x() + m[0][1] * v.y() + m[0][2] * v.z() + m[0][3];
        float y = m[1][0] * v.x() + m[1][1] * v.y() + m[1][2] * v.z() + m[1][3];
        float z = m[2][0] * v.x() + m[2][1] * v.y() + m[2][2] * v.z() + m[2][3];
        float w = m[3][0] * v.x() + m[3][1] * v.y() + m[3][2] * v.z() + m[3][3];
        
        if (w != 0.0f && w != 1.0f)
        {
            x /= w;
            y /= w;
            z /= w;
        }
        
        return vec3(x, y, z);
    }

    vec3 transformDirection(const vec3& v) const
    {
        float x = m[0][0] * v.x() + m[0][1] * v.y() + m[0][2] * v.z();
        float y = m[1][0] * v.x() + m[1][1] * v.y() + m[1][2] * v.z();
        float z = m[2][0] * v.x() + m[2][1] * v.y() + m[2][2] * v.z();
        return vec3(x, y, z);
    }

    mat4 transpose() const
    {
        mat4 result;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                result.m[i][j] = m[j][i];
        return result;
    }

    float determinant() const
    {
        float det = 0.0f;
        for (int i = 0; i < 4; i++)
        {
            det += m[0][i] * cofactor(0, i);
        }
        return det;
    }

    mat4 inverse() const
    {
        float det = determinant();
        if (std::abs(det) < 1e-6f)
            return mat4::identity();

        mat4 result;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                result.m[j][i] = cofactor(i, j) / det;
            }
        }
        return result;
    }

private:
    float cofactor(int row, int col) const
    {
        float minor[3][3];
        int mi = 0;
        for (int i = 0; i < 4; i++)
        {
            if (i == row) continue;
            int mj = 0;
            for (int j = 0; j < 4; j++)
            {
                if (j == col) continue;
                minor[mi][mj] = m[i][j];
                mj++;
            }
            mi++;
        }
        
        float det = minor[0][0] * (minor[1][1] * minor[2][2] - minor[1][2] * minor[2][1])
                  - minor[0][1] * (minor[1][0] * minor[2][2] - minor[1][2] * minor[2][0])
                  + minor[0][2] * (minor[1][0] * minor[2][1] - minor[1][1] * minor[2][0]);
        
        return ((row + col) % 2 == 0 ? 1.0f : -1.0f) * det;
    }
};

inline std::ostream& operator<<(std::ostream& out, const mat4& mat)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            out << mat.m[i][j];
            if (j < 3) out << " ";
        }
        if (i < 3) out << "\n";
    }
    return out;
}

#endif //MAT4_H
