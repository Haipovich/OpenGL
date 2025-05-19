#ifndef MYMATH_MAT4_H
#define MYMATH_MAT4_H

#include "vec3.h"
#include "vec4.h"
#include <vector>
#include <cmath>
#include <cstring>

namespace MyMath {

    struct mat4 {
        union {
            float data[16]{};
            vec4 cols[4];
            struct {
                float m00, m10, m20, m30;
                float m01, m11, m21, m31;
                float m02, m12, m22, m32;
                float m03, m13, m23, m33;
            };
        };

        mat4(float diagonal = 1.0f);

        mat4 operator*(const mat4& other) const;
        vec4 operator*(const vec4& v) const;

        const float* value_ptr() const;

        static mat4 identity();
        static mat4 translate(const vec3& v);
        static mat4 rotate(float angleRadians, const vec3& axis);
        static mat4 scale(const vec3& v);
        static mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up);
        static mat4 perspective(float fovyRadians, float aspect, float near, float far);
        static mat4 orthographic(float left, float right, float bottom, float top, float nearVal, float farVal);

        mat4 transposed() const;
        static mat4 inverse() ;
    };

    float radians(float degrees);

    mat4 translate(const mat4& m, const vec3& v);
    mat4 rotate(const mat4& m, float angleRadians, const vec3& axis);
    mat4 scale(const mat4& m, const vec3& v);

} // namespace MyMath

#endif // MYMATH_MAT4_H
