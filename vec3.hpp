#pragma once
#include <cmath>
#include <cstdio>

struct vec3 {
    float x, y, z;

    // Конструкторы
    constexpr vec3() : x(0), y(0), z(0) {}
    constexpr vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Длина вектора
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Квадрат длины (быстрее!)
    constexpr float lengthSq() const {
        return x * x + y * y + z * z;
    }

    // Нормализация
    vec3 normalize() const {
        float len = length();
        if (len > 0) {
            return vec3(x / len, y / len, z / len);
        }
        return *this;
    }

    // Скалярное произведение
    constexpr float dot(const vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Векторное произведение
    vec3 cross(const vec3& other) const {
        return vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // ---- ОПЕРАТОРЫ СРАВНЕНИЯ ----
    constexpr bool operator==(const vec3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    constexpr bool operator!=(const vec3& other) const {
        return !(*this == other);
    }

    // ---- АРИФМЕТИЧЕСКИЕ ОПЕРАТОРЫ ----
    constexpr vec3 operator+(const vec3& other) const {
        return vec3(x + other.x, y + other.y, z + other.z);
    }

    constexpr vec3 operator-(const vec3& other) const {
        return vec3(x - other.x, y - other.y, z - other.z);
    }

    constexpr vec3 operator*(float s) const {
        return vec3(x * s, y * s, z * s);
    }

    constexpr vec3 operator/(float s) const {
        return vec3(x / s, y / s, z / s);
    }

    // Унарный минус
    constexpr vec3 operator-() const {
        return vec3(-x, -y, -z);
    }

    // ---- СОСТАВНЫЕ ОПЕРАТОРЫ ----
    vec3& operator+=(const vec3& other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }

    vec3& operator-=(const vec3& other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }

    vec3& operator*=(float s) {
        x *= s; y *= s; z *= s;
        return *this;
    }

    vec3& operator/=(float s) {
        x /= s; y /= s; z /= s;
        return *this;
    }

    // ---- ДОСТУП ПО ИНДЕКСУ ----
    constexpr float operator[](int i) const {
        return (&x)[i];
    }

    float& operator[](int i) {
        return (&x)[i];
    }

    // ---- ДРУЖЕСТВЕННЫЕ ФУНКЦИИ ----
    friend constexpr vec3 operator*(float s, const vec3& v) {
        return vec3(v.x * s, v.y * s, v.z * s);
    }

    // ---- ПОЛЕЗНЫЕ МЕТОДЫ ----

    // Проверка на ноль
    constexpr bool isZero() const {
        return x == 0 && y == 0 && z == 0;
    }

    // Обнуление
    void zero() {
        x = y = z = 0;
    }

    // Расстояние до другого вектора
    float distanceTo(const vec3& other) const {
        return (*this - other).length();
    }

    // Квадрат расстояния (быстрее!)
    constexpr float distanceToSq(const vec3& other) const {
        return (*this - other).lengthSq();
    }

    // Линейная интерполяция
    vec3 lerp(const vec3& other, float t) const {
        return *this * (1 - t) + other * t;
    }

    // Ограничение значений
    vec3 clamp(float minVal, float maxVal) const {
        return vec3(
            x < minVal ? minVal : (x > maxVal ? maxVal : x),
            y < minVal ? minVal : (y > maxVal ? maxVal : y),
            z < minVal ? minVal : (z > maxVal ? maxVal : z)
        );
    }

    // ---- ВЫВОД ----
    void print() const {
        printf("vec3(%.2f, %.2f, %.2f)\n", x, y, z);
    }

    void print(const char* name) const {
        printf("%s: (%.2f, %.2f, %.2f)\n", name, x, y, z);
    }
    // Деление на вектор (покомпонентное)
    constexpr vec3 operator/(const vec3& other) const {
        return vec3(x / other.x, y / other.y, z / other.z);
    }

    // Умножение на вектор (покомпонентное)
    constexpr vec3 operator*(const vec3& other) const {
        return vec3(x * other.x, y * other.y, z * other.z);
    }
};

// ---- ГЛОБАЛЬНЫЕ ФУНКЦИИ (ВНЕ СТРУКТУРЫ) ----

// Статические константы (заменяют статические методы)
namespace vec3_constants {
    inline vec3 zero() { return vec3(0, 0, 0); }
    inline vec3 one() { return vec3(1, 1, 1); }
    inline vec3 up() { return vec3(0, 1, 0); }
    inline vec3 down() { return vec3(0, -1, 0); }
    inline vec3 right() { return vec3(1, 0, 0); }
    inline vec3 left() { return vec3(-1, 0, 0); }
    inline vec3 forward() { return vec3(0, 0, 1); }
    inline vec3 back() { return vec3(0, 0, -1); }
}

// Глобальные функции-утилиты
inline vec3 normalize(const vec3& v) {
    return v.normalize();
}

inline float dot(const vec3& a, const vec3& b) {
    return a.dot(b);
}

inline vec3 cross(const vec3& a, const vec3& b) {
    return a.cross(b);
}