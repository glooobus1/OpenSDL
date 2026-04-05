#pragma once
#include "vec3.hpp"
#include <cmath>
#include <algorithm>

struct mat4 {
    float m[16] = { 0 };  // Матрица 4x4 в столбцовом порядке (column-major)

    // Конструктор по умолчанию (единичная матрица)
    constexpr mat4() {
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    // Конструктор с массивом
    constexpr mat4(const float data[16]) {
        for (int i = 0; i < 16; i++) m[i] = data[i];
    }

    // Конструктор копирования
    constexpr mat4(const mat4& other) {
        for (int i = 0; i < 16; i++) m[i] = other.m[i];
    }
    

    // Move конструктор
    constexpr mat4(mat4&& other) noexcept {
        for (int i = 0; i < 16; i++) m[i] = other.m[i];
    }

    // Оператор присваивания
    constexpr mat4& operator=(const mat4& other) {
        for (int i = 0; i < 16; i++) m[i] = other.m[i];
        return *this;
    }

    // Move оператор присваивания
    constexpr mat4& operator=(mat4&& other) noexcept {
        for (int i = 0; i < 16; i++) m[i] = other.m[i];
        return *this;
    }

    // Доступ к элементам
    constexpr float& operator[](int i) { return m[i]; }
    constexpr float operator[](int i) const { return m[i]; }

    // Доступ по строке и столбцу
    constexpr float& at(int row, int col) { return m[col * 4 + row]; }
    constexpr float at(int row, int col) const { return m[col * 4 + row]; }

    // Получение указателя на данные (для OpenGL)
    const float* ptr() const { return m; }
    float* ptr() { return m; }

    // Сравнение
    constexpr bool operator==(const mat4& other) const {
        for (int i = 0; i < 16; i++) {
            if (m[i] != other.m[i]) return false;
        }
        return true;
    }

    constexpr bool operator!=(const mat4& other) const {
        return !(*this == other);
    }

    // Умножение матриц (оптимизированная версия)
    mat4 operator*(const mat4& other) const {
        mat4 result;
        const float* a = m;
        const float* b = other.m;
        float* r = result.m;

        // Ручная развертка для скорости
        r[0] = a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3];
        r[1] = a[1] * b[0] + a[5] * b[1] + a[9] * b[2] + a[13] * b[3];
        r[2] = a[2] * b[0] + a[6] * b[1] + a[10] * b[2] + a[14] * b[3];
        r[3] = a[3] * b[0] + a[7] * b[1] + a[11] * b[2] + a[15] * b[3];

        r[4] = a[0] * b[4] + a[4] * b[5] + a[8] * b[6] + a[12] * b[7];
        r[5] = a[1] * b[4] + a[5] * b[5] + a[9] * b[6] + a[13] * b[7];
        r[6] = a[2] * b[4] + a[6] * b[5] + a[10] * b[6] + a[14] * b[7];
        r[7] = a[3] * b[4] + a[7] * b[5] + a[11] * b[6] + a[15] * b[7];

        r[8] = a[0] * b[8] + a[4] * b[9] + a[8] * b[10] + a[12] * b[11];
        r[9] = a[1] * b[8] + a[5] * b[9] + a[9] * b[10] + a[13] * b[11];
        r[10] = a[2] * b[8] + a[6] * b[9] + a[10] * b[10] + a[14] * b[11];
        r[11] = a[3] * b[8] + a[7] * b[9] + a[11] * b[10] + a[15] * b[11];

        r[12] = a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12] * b[15];
        r[13] = a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13] * b[15];
        r[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
        r[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];

        return result;
    }

    // Умножение матрицы на вектор (точка, w=1)
    vec3 operator*(const vec3& v) const {
        return vec3(
            m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12],
            m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13],
            m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14]
        );
    }

    // Умножение с учётом перспективного деления
    vec3 transformPoint(const vec3& v) const {
        float x = m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12];
        float y = m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13];
        float z = m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14];
        float w = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15];

        if (w != 0.0f && w != 1.0f) {
            return vec3(x / w, y / w, z / w);
        }
        return vec3(x, y, z);
    }

    // Транспонирование
    mat4 transpose() const {
        mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i * 4 + j] = m[j * 4 + i];
            }
        }
        return result;
    }

    // Определитель (для обратной матрицы)
    float determinant() const {
        float det = 0.0f;
        det += m[0] * (m[5] * (m[10] * m[15] - m[11] * m[14]) -
            m[6] * (m[9] * m[15] - m[11] * m[13]) +
            m[7] * (m[9] * m[14] - m[10] * m[13]));
        det -= m[1] * (m[4] * (m[10] * m[15] - m[11] * m[14]) -
            m[6] * (m[8] * m[15] - m[11] * m[12]) +
            m[7] * (m[8] * m[14] - m[10] * m[12]));
        det += m[2] * (m[4] * (m[9] * m[15] - m[11] * m[13]) -
            m[5] * (m[8] * m[15] - m[11] * m[12]) +
            m[7] * (m[8] * m[13] - m[9] * m[12]));
        det -= m[3] * (m[4] * (m[9] * m[14] - m[10] * m[13]) -
            m[5] * (m[8] * m[14] - m[10] * m[12]) +
            m[6] * (m[8] * m[13] - m[9] * m[12]));
        return det;
    }

    // Обратная матрица
    mat4 inverse() const {
        float det = determinant();
        if (std::abs(det) < 1e-6f) return mat4();  // Возвращаем единичную, если определитель близок к 0

        float invDet = 1.0f / det;

        mat4 inv;
        inv.m[0] = (m[5] * (m[10] * m[15] - m[11] * m[14]) -
            m[6] * (m[9] * m[15] - m[11] * m[13]) +
            m[7] * (m[9] * m[14] - m[10] * m[13])) * invDet;
        inv.m[1] = -(m[1] * (m[10] * m[15] - m[11] * m[14]) -
            m[2] * (m[9] * m[15] - m[11] * m[13]) +
            m[3] * (m[9] * m[14] - m[10] * m[13])) * invDet;
        inv.m[2] = (m[1] * (m[6] * m[15] - m[7] * m[14]) -
            m[2] * (m[5] * m[15] - m[7] * m[13]) +
            m[3] * (m[5] * m[14] - m[6] * m[13])) * invDet;
        inv.m[3] = -(m[1] * (m[6] * m[11] - m[7] * m[10]) -
            m[2] * (m[5] * m[11] - m[7] * m[9]) +
            m[3] * (m[5] * m[10] - m[6] * m[9])) * invDet;

        inv.m[4] = -(m[4] * (m[10] * m[15] - m[11] * m[14]) -
            m[6] * (m[8] * m[15] - m[11] * m[12]) +
            m[7] * (m[8] * m[14] - m[10] * m[12])) * invDet;
        inv.m[5] = (m[0] * (m[10] * m[15] - m[11] * m[14]) -
            m[2] * (m[8] * m[15] - m[11] * m[12]) +
            m[3] * (m[8] * m[14] - m[10] * m[12])) * invDet;
        inv.m[6] = -(m[0] * (m[6] * m[15] - m[7] * m[14]) -
            m[2] * (m[4] * m[15] - m[7] * m[12]) +
            m[3] * (m[4] * m[14] - m[6] * m[12])) * invDet;
        inv.m[7] = (m[0] * (m[6] * m[11] - m[7] * m[10]) -
            m[2] * (m[4] * m[11] - m[7] * m[8]) +
            m[3] * (m[4] * m[10] - m[6] * m[8])) * invDet;

        inv.m[8] = (m[4] * (m[9] * m[15] - m[11] * m[13]) -
            m[5] * (m[8] * m[15] - m[11] * m[12]) +
            m[7] * (m[8] * m[13] - m[9] * m[12])) * invDet;
        inv.m[9] = -(m[0] * (m[9] * m[15] - m[11] * m[13]) -
            m[1] * (m[8] * m[15] - m[11] * m[12]) +
            m[3] * (m[8] * m[13] - m[9] * m[12])) * invDet;
        inv.m[10] = (m[0] * (m[5] * m[15] - m[7] * m[13]) -
            m[1] * (m[4] * m[15] - m[7] * m[12]) +
            m[3] * (m[4] * m[13] - m[5] * m[12])) * invDet;
        inv.m[11] = -(m[0] * (m[5] * m[11] - m[7] * m[9]) -
            m[1] * (m[4] * m[11] - m[7] * m[8]) +
            m[3] * (m[4] * m[9] - m[5] * m[8])) * invDet;

        inv.m[12] = -(m[4] * (m[9] * m[14] - m[10] * m[13]) -
            m[5] * (m[8] * m[14] - m[10] * m[12]) +
            m[6] * (m[8] * m[13] - m[9] * m[12])) * invDet;
        inv.m[13] = (m[0] * (m[9] * m[14] - m[10] * m[13]) -
            m[1] * (m[8] * m[14] - m[10] * m[12]) +
            m[2] * (m[8] * m[13] - m[9] * m[12])) * invDet;
        inv.m[14] = -(m[0] * (m[5] * m[14] - m[6] * m[13]) -
            m[1] * (m[4] * m[14] - m[6] * m[12]) +
            m[2] * (m[4] * m[13] - m[5] * m[12])) * invDet;
        inv.m[15] = (m[0] * (m[5] * m[10] - m[6] * m[9]) -
            m[1] * (m[4] * m[10] - m[6] * m[8]) +
            m[2] * (m[4] * m[9] - m[5] * m[8])) * invDet;

        return inv;
    }

    // ---- СТАТИЧЕСКИЕ МЕТОДЫ ДЛЯ СОЗДАНИЯ МАТРИЦ ----

    // Единичная матрица
    static constexpr mat4 identity() {
        return mat4();
    }

    // Матрица перемещения
    static constexpr mat4 translation(const vec3& offset) {
        mat4 result;
        result.m[12] = offset.x;
        result.m[13] = offset.y;
        result.m[14] = offset.z;
        return result;
    }

    // Матрица масштабирования
    static constexpr mat4 scale(const vec3& factor) {
        mat4 result;
        result.m[0] = factor.x;
        result.m[5] = factor.y;
        result.m[10] = factor.z;
        return result;
    }

    // Матрица поворота вокруг оси X
    static mat4 rotationX(float angle) {
        float c = cos(angle);
        float s = sin(angle);

        mat4 result;
        result.m[5] = c;
        result.m[6] = s;
        result.m[9] = -s;
        result.m[10] = c;
        return result;
    }

    // Матрица поворота вокруг оси Y (ИСПРАВЛЕНО)
    static mat4 rotationY(float angle) {
        float c = cos(angle);
        float s = sin(angle);

        mat4 result;
        result.m[0] = c;
        result.m[2] = -s;      // Исправлено
        result.m[8] = s;       // Исправлено
        result.m[10] = c;
        return result;
    }

    // Матрица поворота вокруг оси Z
    static mat4 rotationZ(float angle) {
        float c = cos(angle);
        float s = sin(angle);

        mat4 result;
        result.m[0] = c;
        result.m[1] = s;
        result.m[4] = -s;
        result.m[5] = c;
        return result;
    }

    // Матрица поворота вокруг произвольной оси
    static mat4 rotation(float angle, const vec3& axis) {
        vec3 a = axis;
        float len = a.length();
        if (len > 0.0001f) {
            a = a / len;  // Нормализация
        }

        float c = cos(angle);
        float s = sin(angle);
        float t = 1.0f - c;

        mat4 result;

        result.m[0] = t * a.x * a.x + c;
        result.m[1] = t * a.x * a.y + s * a.z;
        result.m[2] = t * a.x * a.z - s * a.y;

        result.m[4] = t * a.x * a.y - s * a.z;
        result.m[5] = t * a.y * a.y + c;
        result.m[6] = t * a.y * a.z + s * a.x;

        result.m[8] = t * a.x * a.z + s * a.y;
        result.m[9] = t * a.y * a.z - s * a.x;
        result.m[10] = t * a.z * a.z + c;

        return result;
    }

    // Матрица ортографической проекции
    static mat4 ortho(float left, float right, float bottom, float top, float near, float far) {
        mat4 result;

        result.m[0] = 2.0f / (right - left);
        result.m[5] = 2.0f / (top - bottom);
        result.m[10] = -2.0f / (far - near);

        result.m[12] = -(right + left) / (right - left);
        result.m[13] = -(top + bottom) / (top - bottom);
        result.m[14] = -(far + near) / (far - near);

        return result;
    }

    // Матрица перспективной проекции
    static mat4 perspective(float fov, float aspect, float near, float far) {
        float tanHalfFov = tan(fov / 2.0f);

        mat4 result;
        result.m[0] = 1.0f / (aspect * tanHalfFov);
        result.m[5] = 1.0f / tanHalfFov;
        result.m[10] = -(far + near) / (far - near);
        result.m[11] = -1.0f;
        result.m[14] = -(2.0f * far * near) / (far - near);
        result.m[15] = 0.0f;

        return result;
    }

    // Матрица LookAt (для камеры)
    static mat4 lookAt(const vec3& eye, const vec3& target, const vec3& up) {
        vec3 z = (eye - target).normalize();
        vec3 x = up.cross(z).normalize();
        vec3 y = z.cross(x);

        mat4 result;

        result.m[0] = x.x;
        result.m[1] = y.x;
        result.m[2] = z.x;

        result.m[4] = x.y;
        result.m[5] = y.y;
        result.m[6] = z.y;

        result.m[8] = x.z;
        result.m[9] = y.z;
        result.m[10] = z.z;

        result.m[12] = -x.dot(eye);
        result.m[13] = -y.dot(eye);
        result.m[14] = -z.dot(eye);

        return result;
    }

    // ---- ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ----

    // Печать матрицы
    void print() const {
        printf("mat4:\n");
        printf("%.2f %.2f %.2f %.2f\n", m[0], m[4], m[8], m[12]);
        printf("%.2f %.2f %.2f %.2f\n", m[1], m[5], m[9], m[13]);
        printf("%.2f %.2f %.2f %.2f\n", m[2], m[6], m[10], m[14]);
        printf("%.2f %.2f %.2f %.2f\n", m[3], m[7], m[11], m[15]);
    }

    void print(const char* name) const {
        printf("%s:\n", name);
        printf("%.2f %.2f %.2f %.2f\n", m[0], m[4], m[8], m[12]);
        printf("%.2f %.2f %.2f %.2f\n", m[1], m[5], m[9], m[13]);
        printf("%.2f %.2f %.2f %.2f\n", m[2], m[6], m[10], m[14]);
        printf("%.2f %.2f %.2f %.2f\n", m[3], m[7], m[11], m[15]);
    }
};

// ---- ГЛОБАЛЬНЫЕ ОПЕРАТОРЫ ДЛЯ УДОБСТВА ----

// Умножение матрицы на скаляр
inline mat4 operator*(const mat4& m, float s) {
    mat4 result;
    for (int i = 0; i < 16; i++) result[i] = m[i] * s;
    return result;
}

// Умножение скаляра на матрицу
inline mat4 operator*(float s, const mat4& m) {
    return m * s;
}

// Сложение матриц
inline mat4 operator+(const mat4& a, const mat4& b) {
    mat4 result;
    for (int i = 0; i < 16; i++) result[i] = a[i] + b[i];
    return result;
}

// Вычитание матриц
inline mat4 operator-(const mat4& a, const mat4& b) {
    mat4 result;
    for (int i = 0; i < 16; i++) result[i] = a[i] - b[i];
    return result;
}