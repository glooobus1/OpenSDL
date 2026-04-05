#pragma once
#include <string>
#include <cstdint>
#include "color.hpp"

class Texture {
private:
    unsigned int textureId;
    int width;
    int height;
    int channels;

public:
    Texture();
    ~Texture();

    // Загрузка из файла (поддерживает BMP, PNG, JPG через SDL_image или stb_image)
    bool loadFromFile(const std::string& filename);

    // Создание пустой текстуры
    bool createEmpty(int w, int h, int channels = 4);

    // Загрузка из памяти
    bool loadFromMemory(const uint8_t* data, int w, int h, int channels);

    // Использование текстуры
    void bind(int unit = 0) const;
    void unbind() const;

    // Настройка параметров
    void setFiltering(bool linear);
    void setWrapping(bool repeat);

    // Геттеры
    unsigned int getId() const { return textureId; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getChannels() const { return channels; }

    // Статические методы для создания простых текстур
    static Texture createCheckerboard(int size = 64, const Color& c1 = Color::white(), const Color& c2 = Color::black());
    static Texture createGradient(int width, int height);
};