#ifndef MYMATH_VEC3_H
#define MYMATH_VEC3_H

#include <cmath>
#include <iostream>

namespace MyMath {

    struct vec3 {
        float x = 0.0f, y = 0.0f, z = 0.0f;

        vec3() = default;
        vec3(float scalar);
        vec3(float x, float y, float z);

        vec3 operator+(const vec3& other) const;
        vec3 operator-(const vec3& other) const;
        vec3 operator*(float scalar) const;
        vec3 operator-() const;
        vec3& operator+=(const vec3& other);
        vec3& operator-=(const vec3& other);
        vec3& operator*=(float scalar);

        float length() const;
        float lengthSquared() const;
        vec3 normalized() const;
        void normalize();

        friend vec3 operator*(float scalar, const vec3& v);
    };

    float dot(const vec3& a, const vec3& b);
    vec3 cross(const vec3& a, const vec3& b);
    vec3 normalize(const vec3& v);
    std::ostream& operator<<(std::ostream& os, const vec3& v);

} // namespace MyMath

#endif // MYMATH_VEC3_H