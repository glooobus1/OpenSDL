#pragma once
#include "vec4.hpp"
#include <cstdint>

struct Color {
    union {
        struct { float r, g, b, a; };
        vec4 v;
    };

    // Конструкторы
    constexpr Color() : r(0), g(0), b(0), a(1) {}
    constexpr Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

    // Из 0-255 в 0-1
    static Color fromRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }

    // Из HEX (например 0xFF0000 для красного)
    static Color fromHEX(uint32_t hex, float alpha = 1.0f) {
        return Color(
            ((hex >> 16) & 0xFF) / 255.0f,
            ((hex >> 8) & 0xFF) / 255.0f,
            (hex & 0xFF) / 255.0f,
            alpha
        );
    }

    // Предопределённые цвета
    static constexpr Color white() { return Color(1, 1, 1, 1); }
    static constexpr Color black() { return Color(0, 0, 0, 1); }
    static constexpr Color red() { return Color(1, 0, 0, 1); }
    static constexpr Color green() { return Color(0, 1, 0, 1); }
    static constexpr Color blue() { return Color(0, 0, 1, 1); }
    static constexpr Color yellow() { return Color(1, 1, 0, 1); }
    static constexpr Color cyan() { return Color(0, 1, 1, 1); }
    static constexpr Color magenta() { return Color(1, 0, 1, 1); }
    static constexpr Color transparent() { return Color(0, 0, 0, 0); }

    // Операторы
    Color operator+(const Color& other) const {
        return Color(r + other.r, g + other.g, b + other.b, a + other.a);
    }

    Color operator*(float s) const {
        return Color(r * s, g * s, b * s, a * s);
    }

    // Смешивание цветов
    Color mix(const Color& other, float t) const {
        return Color(
            r + (other.r - r) * t,
            g + (other.g - g) * t,
            b + (other.b - b) * t,
            a + (other.a - a) * t
        );
    }

    // Яркость (0-1)
    float luminance() const {
        return 0.2126f * r + 0.7152f * g + 0.0722f * b;
    }

    // В vec4 (для шейдеров)
    constexpr const vec4& toVec4() const { return v; }

    // Печать
    void print() const {
        printf("Color(%.2f, %.2f, %.2f, %.2f)\n", r, g, b, a);
    }

    void print(const char* name) const {
        printf("%s: (%.2f, %.2f, %.2f, %.2f)\n", name, r, g, b, a);
    }
};