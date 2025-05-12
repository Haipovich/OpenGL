#include "MyMath/vec3.h"
#include <limits>

namespace MyMath {

    vec3::vec3(float scalar) : x(scalar), y(scalar), z(scalar) {}
    vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    vec3 vec3::operator+(const vec3& other) const {
        return vec3(x + other.x, y + other.y, z + other.z);
    }

    vec3 vec3::operator-(const vec3& other) const {
        return vec3(x - other.x, y - other.y, z - other.z);
    }

    vec3 vec3::operator*(float scalar) const {
        return vec3(x * scalar, y * scalar, z * scalar);
    }

    vec3 vec3::operator-() const {
        return vec3(-x, -y, -z);
    }

    vec3& vec3::operator+=(const vec3& other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }

    vec3& vec3::operator-=(const vec3& other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }

    vec3& vec3::operator*=(float scalar) {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }

    float vec3::lengthSquared() const {
        return x * x + y * y + z * z;
    }

    float vec3::length() const {
        return std::sqrt(lengthSquared());
    }

    vec3 vec3::normalized() const {
        float l = length();
        if (l > std::numeric_limits<float>::epsilon()) {
            return vec3(x / l, y / l, z / l);
        }
        return vec3(0.0f);
    }

    void vec3::normalize() {
        float l = length();
        if (l > std::numeric_limits<float>::epsilon()) {
            x /= l;
            y /= l;
            z /= l;
        } else {
            x = y = z = 0.0f;
        }
    }

    vec3 operator*(float scalar, const vec3& v) {
        return v * scalar;
    }

    float dot(const vec3& a, const vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    vec3 cross(const vec3& a, const vec3& b) {
        return vec3(
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
        );
    }

    vec3 normalize(const vec3& v) {
        return v.normalized();
    }

    std::ostream& operator<<(std::ostream& os, const vec3& v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }

} // namespace MyMath