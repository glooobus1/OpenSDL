#include "texture.hpp"
#include <GL/glew.h>
#include <iostream>

// Конструктор
Texture::Texture() : textureId(0), width(0), height(0), channels(0) {
    // Пустой конструктор
}

// Деструктор
Texture::~Texture() {
    if (textureId != 0) {
        glDeleteTextures(1, &textureId);
    }
}

// Загрузка из файла (заглушка)
bool Texture::loadFromFile(const std::string& filename) {
    std::cout << "Texture::loadFromFile not implemented yet: " << filename << std::endl;
    return false;
}

// Создание пустой текстуры
bool Texture::createEmpty(int w, int h, int channels) {
    width = w;
    height = h;
    this->channels = channels;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

// Загрузка из памяти
bool Texture::loadFromMemory(const uint8_t* data, int w, int h, int channels) {
    width = w;
    height = h;
    this->channels = channels;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

// Использование текстуры
void Texture::bind(int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Настройка параметров
void Texture::setFiltering(bool linear) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
}

void Texture::setWrapping(bool repeat) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP);
}

// Статические методы (заглушки)
Texture Texture::createCheckerboard(int size, const Color& c1, const Color& c2) {
    Texture tex;
    tex.createEmpty(size, size, 3);
    std::cout << "Checkerboard texture created (simplified)" << std::endl;
    return tex;
}

Texture Texture::createGradient(int width, int height) {
    Texture tex;
    tex.createEmpty(width, height, 3);
    std::cout << "Gradient texture created (simplified)" << std::endl;
    return tex;
}