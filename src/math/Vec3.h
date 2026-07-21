#ifndef VEC3_H
#define VEC3_H

#include <cmath>

class Vec3 {
public:
    float x, y, z;

    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float xVal, float yVal, float zVal) : x(xVal), y(yVal), z(zVal) {}

    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    Vec3 operator/(float scalar) const {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // returns unit vector (length = 1.0) pointing in same direction
    Vec3 normalize() const {
        float len = length();
        if (len > 0.0f) {
            return *this / len;
        }
        return Vec3(0.0f, 0.0f, 0.0f);
    }

    static float dot(const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return Vec3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }
};

#endif
