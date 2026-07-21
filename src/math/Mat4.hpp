#ifndef MAT4_HPP
#define MAT4_HPP

#include "Vec3.hpp"
#include <cmath>

class Mat4 {
public:
    float m[16];

    Mat4() {
        for (int i = 0; i < 16; ++i)
            m[i] = 0.0f;
        m[0] = 1.0f;
        m[5] = 1.0f;
        m[10] = 1.0f;
        m[15] = 1.0f;
    }

    static Mat4 identity() {
        return Mat4();
    }
    Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                result.m[col * 4 + row] =
                    m[0 * 4 + row] * other.m[col * 4 + 0] +
                    m[1 * 4 + row] * other.m[col * 4 + 1] +
                    m[2 * 4 + row] * other.m[col * 4 + 2] +
                    m[3 * 4 + row] * other.m[col * 4 + 3];
            }
        }
        return result;
    }
    static Mat4 translate(const Vec3& offset) {
        Mat4 result = identity();
        result.m[12] = offset.x;
        result.m[13] = offset.y;
        result.m[14] = offset.z;
        return result;
    }
    static Mat4 scale(const Vec3& s) {
        Mat4 result;
        result.m[0] = s.x;
        result.m[5] = s.y;
        result.m[10] = s.z;
        result.m[15] = 1.0f;
        return result;
    }

    //perspective projection , ratio = width / height (fov in degrees)
    static Mat4 perspective(float fovDegrees, float aspect, float nearPlane, float farPlane) {
        Mat4 result;
        float fovRad = fovDegrees * (3.1415926535f / 180.0f);
        float tanHalfFov = std::tan(fovRad / 2.0f);

        result.m[0] = 1.0f / (aspect * tanHalfFov);
        result.m[5] = 1.0f / tanHalfFov;
        result.m[10] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        result.m[11] = -1.0f;
        result.m[14] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
        result.m[15] = 0.0f;
        return result;
    }

    // constructs camera view transform from position (eye), target (center), and up vector
    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = (center - eye).normalize();     // Forward direction
        Vec3 s = Vec3::cross(f, up).normalize(); // Right direction
        Vec3 u = Vec3::cross(s, f);              // True Up direction

        Mat4 result = identity();
        result.m[0] = s.x;  result.m[4] = s.y;  result.m[8]  = s.z;
        result.m[1] = u.x;  result.m[5] = u.y;  result.m[9]  = u.z;
        result.m[2] = -f.x; result.m[6] = -f.y; result.m[10] = -f.z;

        result.m[12] = -Vec3::dot(s, eye);
        result.m[13] = -Vec3::dot(u, eye);
        result.m[14] =  Vec3::dot(f, eye);
        return result;
    }
};

#endif
