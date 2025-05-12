#ifndef MYMATH_VEC4_H
#define MYMATH_VEC4_H

#include "vec3.h"

namespace MyMath {

    struct vec4 {
        float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;

        vec4() = default;
        vec4(float scalar);
        vec4(float x, float y, float z, float w);
        vec4(const vec3& v, float w);
    };

    inline vec4::vec4(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
    inline vec4::vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    inline vec4::vec4(const vec3& v, float w_val) : x(v.x), y(v.y), z(v.z), w(w_val) {}
} // namespace MyMath

#endif // MYMATH_VEC4_H