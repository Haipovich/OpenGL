#include "MyMath/mat4.h"
#include <stdexcept>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace MyMath {
    mat4::mat4(float diagonal) {
        std::memset(data, 0, 16 * sizeof(float));
        m00 = m11 = m22 = m33 = diagonal;
    }

    const float* mat4::value_ptr() const {
        return data;
    }

    mat4 mat4::operator*(const mat4& other) const {
        mat4 result(0.0f);
        const mat4& a = *this;
        const mat4& b = other;

        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    sum += a.data[k * 4 + row] * b.data[col * 4 + k];
                }
                result.data[col * 4 + row] = sum;
            }
        }
        return result;
    }

    vec4 mat4::operator*(const vec4& v) const {
        vec4 result;
        result.x = m00 * v.x + m01 * v.y + m02 * v.z + m03 * v.w;
        result.y = m10 * v.x + m11 * v.y + m12 * v.z + m13 * v.w;
        result.z = m20 * v.x + m21 * v.y + m22 * v.z + m23 * v.w;
        result.w = m30 * v.x + m31 * v.y + m32 * v.z + m33 * v.w;
        return result;
    }

    mat4 mat4::identity() {
        return mat4(1.0f);
    }

    mat4 mat4::translate(const vec3& v) {
        mat4 result(1.0f);
        result.m03 = v.x;
        result.m13 = v.y;
        result.m23 = v.z;
        return result;
    }

    mat4 mat4::scale(const vec3& v) {
        mat4 result(0.0f);
        result.m00 = v.x;
        result.m11 = v.y;
        result.m22 = v.z;
        result.m33 = 1.0f;
        return result;
    }

    mat4 mat4::rotate(float angleRadians, const vec3& axis) {
        vec3 a = normalize(axis);
        float s = std::sin(angleRadians);
        float c = std::cos(angleRadians);
        float oc = 1.0f - c;

        mat4 result(0.0f);

        result.m00 = c + a.x * a.x * oc;
        result.m10 = a.y * a.x * oc + a.z * s;
        result.m20 = a.z * a.x * oc - a.y * s;
        result.m01 = a.x * a.y * oc - a.z * s;
        result.m11 = c + a.y * a.y * oc;
        result.m21 = a.z * a.y * oc + a.x * s;
        result.m02 = a.x * a.z * oc + a.y * s;
        result.m12 = a.y * a.z * oc - a.x * s;
        result.m22 = c + a.z * a.z * oc;
        result.m33 = 1.0f;

        return result;
    }

// Матрица вида (View Matrix)
    mat4 mat4::lookAt(const vec3& eye, const vec3& center, const vec3& up) {
        vec3 f = normalize(center - eye);
        vec3 s = normalize(cross(f, up));
        vec3 u = cross(s, f);
        mat4 result(1.0f);

        result.m00 = s.x;  result.m01 = s.y;  result.m02 = s.z;
        result.m10 = u.x;  result.m11 = u.y;  result.m12 = u.z;
        result.m20 = -f.x; result.m21 = -f.y; result.m22 = -f.z;

        result.m03 = -dot(s, eye);
        result.m13 = -dot(u, eye);
        result.m23 = dot(f, eye);

        return result;
    }

    mat4 mat4::perspective(float fovyRadians, float aspect, float zNear, float zFar) {
        if (aspect <= 0 || zNear <= 0 || zFar <= zNear || fovyRadians <= 0 || fovyRadians >= M_PI) {
            throw std::invalid_argument("Invalid parameters for perspective matrix");
        }

        float tanHalfFovy = std::tan(fovyRadians / 2.0f);

        mat4 result(0.0f);

        result.m00 = 1.0f / (aspect * tanHalfFovy);
        result.m11 = 1.0f / (tanHalfFovy);
        result.m22 = -(zFar + zNear) / (zFar - zNear);
        result.m32 = -1.0f;
        result.m23 = -(2.0f * zFar * zNear) / (zFar - zNear);

        return result;
    }

    mat4 mat4::transposed() const {
        mat4 result(0.0f);
        result.m00 = m00; result.m01 = m10; result.m02 = m20; result.m03 = m30;
        result.m10 = m01; result.m11 = m11; result.m12 = m21; result.m13 = m31;
        result.m20 = m02; result.m21 = m12; result.m22 = m22; result.m23 = m32;
        result.m30 = m03; result.m31 = m13; result.m32 = m23; result.m33 = m33;
        return result;
    }

    mat4 mat4::inverse() {
        return mat4::identity();
    }

    float radians(float degrees) {
        return degrees * static_cast<float>(M_PI) / 180.0f;
    }

    mat4 translate(const mat4& m, const vec3& v) {
        return m * mat4::translate(v);
    }

    mat4 rotate(const mat4& m, float angleRadians, const vec3& axis) {
        return m * mat4::rotate(angleRadians, axis);
    }

    mat4 scale(const mat4& m, const vec3& v) {
        return m * mat4::scale(v);
    }


} // namespace MyMath