#pragma once
#include <cmath>
#include <cstdio>

struct vec2 {
    float x, y;

    // Конструкторы
    constexpr vec2() : x(0), y(0) {}
    constexpr vec2(float x, float y) : x(x), y(y) {}

    // Операторы
    constexpr vec2 operator+(const vec2& other) const {
        return vec2(x + other.x, y + other.y);
    }

    constexpr vec2 operator-(const vec2& other) const {
        return vec2(x - other.x, y - other.y);
    }

    constexpr vec2 operator*(float s) const {
        return vec2(x * s, y * s);
    }

    constexpr vec2 operator/(float s) const {
        return vec2(x / s, y / s);
    }

    vec2& operator+=(const vec2& other) {
        x += other.x; y += other.y;
        return *this;
    }

    vec2& operator-=(const vec2& other) {
        x -= other.x; y -= other.y;
        return *this;
    }

    vec2& operator*=(float s) {
        x *= s; y *= s;
        return *this;
    }

    vec2& operator/=(float s) {
        x /= s; y /= s;
        return *this;
    }

    constexpr bool operator==(const vec2& other) const {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(const vec2& other) const {
        return !(*this == other);
    }

    // Длина
    float length() const {
        return std::sqrt(x * x + y * y);
    }

    constexpr float lengthSq() const {
        return x * x + y * y;
    }

    // Нормализация
    vec2 normalize() const {
        float len = length();
        if (len > 0) {
            return vec2(x / len, y / len);
        }
        return *this;
    }

    // Скалярное произведение
    constexpr float dot(const vec2& other) const {
        return x * other.x + y * other.y;
    }

    // Векторное произведение (для 2D даёт скаляр)
    constexpr float cross(const vec2& other) const {
        return x * other.y - y * other.x;
    }

    // Поворот на угол (в радианах)
    vec2 rotate(float angle) const {
        float c = cos(angle);
        float s = sin(angle);
        return vec2(x * c - y * s, x * s + y * c);
    }

    // Угол вектора
    float angle() const {
        return atan2(y, x);
    }

    // Линейная интерполяция
    vec2 lerp(const vec2& other, float t) const {
        return *this * (1 - t) + other * t;
    }

    // Статические методы
    static constexpr vec2 zero() { return vec2(0, 0); }
    static constexpr vec2 one() { return vec2(1, 1); }
    static constexpr vec2 up() { return vec2(0, 1); }
    static constexpr vec2 down() { return vec2(0, -1); }
    static constexpr vec2 right() { return vec2(1, 0); }
    static constexpr vec2 left() { return vec2(-1, 0); }

    // Печать
    void print() const {
        printf("vec2(%.2f, %.2f)\n", x, y);
    }

    void print(const char* name) const {
        printf("%s: (%.2f, %.2f)\n", name, x, y);
    }
};

// Глобальные операторы
inline constexpr vec2 operator*(float s, const vec2& v) {
    return vec2(v.x * s, v.y * s);
}