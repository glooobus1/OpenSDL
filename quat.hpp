#pragma once
#include "vec3.hpp"
#include "mat4.hpp"
#include <cmath>

struct quat {
    float x, y, z, w;

    // Конструкторы
    constexpr quat() : x(0), y(0), z(0), w(1) {}  // единичный кватернион

    constexpr quat(float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w) {
    }

    // Создание из угла и оси
    static quat fromAxisAngle(const vec3& axis, float angle) {
        vec3 normalized = axis.normalize();
        float halfAngle = angle * 0.5f;
        float s = sin(halfAngle);

        return quat(
            normalized.x * s,
            normalized.y * s,
            normalized.z * s,
            cos(halfAngle)
        );
    }

    // Создание из углов Эйлера (XYZ)
    static quat fromEuler(float pitch, float yaw, float roll) {
        float cy = cos(yaw * 0.5f);
        float sy = sin(yaw * 0.5f);
        float cp = cos(pitch * 0.5f);
        float sp = sin(pitch * 0.5f);
        float cr = cos(roll * 0.5f);
        float sr = sin(roll * 0.5f);

        quat q;
        q.w = cr * cp * cy + sr * sp * sy;
        q.x = sr * cp * cy - cr * sp * sy;
        q.y = cr * sp * cy + sr * cp * sy;
        q.z = cr * cp * sy - sr * sp * cy;
        return q;
    }

    // Длина кватерниона
    float length() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    // Нормализация
    quat normalize() const {
        float len = length();
        if (len > 0) {
            return quat(x / len, y / len, z / len, w / len);
        }
        return *this;
    }

    // Сопряжённый кватернион (обратный для единичного)
    quat conjugate() const {
        return quat(-x, -y, -z, w);
    }

    // Обратный кватернион
    quat inverse() const {
        float lenSq = x * x + y * y + z * z + w * w;
        if (lenSq > 0) {
            float inv = 1.0f / lenSq;
            return quat(-x * inv, -y * inv, -z * inv, w * inv);
        }
        return *this;
    }

    // Умножение кватернионов (композиция вращений)
    quat operator*(const quat& other) const {
        return quat(
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w,
            w * other.w - x * other.x - y * other.y - z * other.z
        );
    }

    // Поворот вектора кватернионом
    vec3 operator*(const vec3& v) const {
        // q * v * q^(-1)
        quat vecQ(v.x, v.y, v.z, 0);
        quat result = (*this) * vecQ * this->conjugate();
        return vec3(result.x, result.y, result.z);
    }

    // Скалярное произведение
    float dot(const quat& other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    // Линейная интерполяция (LERP) - быстрая
    quat lerp(const quat& other, float t) const {
        return quat(
            x + (other.x - x) * t,
            y + (other.y - y) * t,
            z + (other.z - z) * t,
            w + (other.w - w) * t
        ).normalize();
    }

    // Сферическая интерполяция (SLERP) - плавная
    quat slerp(const quat& other, float t) const {
        float dot = this->dot(other);

        // Если угол маленький, используем LERP
        if (dot > 0.9995f) return lerp(other, t);

        dot = std::clamp(dot, -1.0f, 1.0f);
        float theta = std::acos(dot) * t;

        quat q2 = (other - (*this) * dot).normalize();
        return (*this) * std::cos(theta) + q2 * std::sin(theta);
    }

    // Преобразование в матрицу 4x4
    mat4 toMatrix() const {
        mat4 m;

        float xx = x * x;
        float yy = y * y;
        float zz = z * z;
        float xy = x * y;
        float xz = x * z;
        float yz = y * z;
        float wx = w * x;
        float wy = w * y;
        float wz = w * z;

        m.m[0] = 1.0f - 2.0f * (yy + zz);
        m.m[1] = 2.0f * (xy + wz);
        m.m[2] = 2.0f * (xz - wy);

        m.m[4] = 2.0f * (xy - wz);
        m.m[5] = 1.0f - 2.0f * (xx + zz);
        m.m[6] = 2.0f * (yz + wx);

        m.m[8] = 2.0f * (xz + wy);
        m.m[9] = 2.0f * (yz - wx);
        m.m[10] = 1.0f - 2.0f * (xx + yy);

        return m;
    }

    // Получение угла и оси
    void getAxisAngle(vec3& axis, float& angle) const {
        angle = 2.0f * std::acos(w);
        float s = std::sqrt(1.0f - w * w);
        if (s < 0.001f) {
            axis = vec3(1, 0, 0);
        }
        else {
            axis = vec3(x / s, y / s, z / s);
        }
    }

    // Вектор вперёд (для камеры)
    vec3 forward() const {
        return (*this) * vec3(0, 0, 1);
    }

    // Вектор вправо
    vec3 right() const {
        return (*this) * vec3(1, 0, 0);
    }

    // Вектор вверх
    vec3 up() const {
        return (*this) * vec3(0, 1, 0);
    }

    // Операторы
    quat operator+(const quat& other) const {
        return quat(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    quat operator-(const quat& other) const {
        return quat(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    quat operator*(float s) const {
        return quat(x * s, y * s, z * s, w * s);
    }

    friend quat operator*(float s, const quat& q) {
        return q * s;
    }

    // Печать
    void print() const {
        printf("quat(%.3f, %.3f, %.3f, %.3f)\n", x, y, z, w);
    }

    void print(const char* name) const {
        printf("%s: (%.3f, %.3f, %.3f, %.3f)\n", name, x, y, z, w);
    }
};

// Дополнительные функции
inline quat normalize(const quat& q) { return q.normalize(); }
inline quat conjugate(const quat& q) { return q.conjugate(); }
inline quat inverse(const quat& q) { return q.inverse(); }
inline float dot(const quat& a, const quat& b) { return a.dot(b); }