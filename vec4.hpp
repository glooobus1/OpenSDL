#pragma once
#include "vec3.hpp"
#include <cmath>
#include <cstdio>

struct vec4 {
    float x, y, z, w;

    // Конструкторы
    constexpr vec4() : x(0), y(0), z(0), w(0) {}
    constexpr vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    constexpr vec4(const vec3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
    constexpr vec4(const vec3& v) : x(v.x), y(v.y), z(v.z), w(1.0f) {} // Точка в 3D

    // Операторы
    constexpr vec4 operator+(const vec4& other) const {
        return vec4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    constexpr vec4 operator-(const vec4& other) const {
        return vec4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    constexpr vec4 operator*(float s) const {
        return vec4(x * s, y * s, z * s, w * s);
    }

    constexpr vec4 operator/(float s) const {
        return vec4(x / s, y / s, z / s, w / s);
    }

    vec4& operator+=(const vec4& other) {
        x += other.x; y += other.y; z += other.z; w += other.w;
        return *this;
    }

    vec4& operator-=(const vec4& other) {
        x -= other.x; y -= other.y; z -= other.z; w -= other.w;
        return *this;
    }

    vec4& operator*=(float s) {
        x *= s; y *= s; z *= s; w *= s;
        return *this;
    }

    vec4& operator/=(float s) {
        x /= s; y /= s; z /= s; w /= s;
        return *this;
    }

    constexpr bool operator==(const vec4& other) const {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    constexpr bool operator!=(const vec4& other) const {
        return !(*this == other);
    }

    // Доступ по индексу
    constexpr float operator[](int i) const {
        return (&x)[i];
    }

    float& operator[](int i) {
        return (&x)[i];
    }

    // Длина
    float length() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    constexpr float lengthSq() const {
        return x * x + y * y + z * z + w * w;
    }

    // Нормализация
    vec4 normalize() const {
        float len = length();
        if (len > 0) {
            return vec4(x / len, y / len, z / len, w / len);
        }
        return *this;
    }

    // Скалярное произведение
    constexpr float dot(const vec4& other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    // Преобразование в vec3 (отбрасываем w)
    constexpr vec3 toVec3() const {
        return vec3(x, y, z);
    }

    // Перспективное деление (для преобразования из clip space)
    vec3 project() const {
        if (w != 0.0f && w != 1.0f) {
            return vec3(x / w, y / w, z / w);
        }
        return vec3(x, y, z);
    }

    // Линейная интерполяция
    vec4 lerp(const vec4& other, float t) const {
        return *this * (1 - t) + other * t;
    }

    // Статические методы
    static constexpr vec4 zero() { return vec4(0, 0, 0, 0); }
    static constexpr vec4 one() { return vec4(1, 1, 1, 1); }
    static constexpr vec4 point(float x, float y, float z) { return vec4(x, y, z, 1); }
    static constexpr vec4 vector(float x, float y, float z) { return vec4(x, y, z, 0); }

    // Печать
    void print() const {
        printf("vec4(%.2f, %.2f, %.2f, %.2f)\n", x, y, z, w);
    }

    void print(const char* name) const {
        printf("%s: (%.2f, %.2f, %.2f, %.2f)\n", name, x, y, z, w);
    }
};

// Глобальные операторы
inline constexpr vec4 operator*(float s, const vec4& v) {
    return vec4(v.x * s, v.y * s, v.z * s, v.w * s);
}