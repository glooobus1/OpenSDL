#pragma once
#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include "color.hpp"
#include "mat4.hpp"

class TextRenderer {
private:
    unsigned int m_VAO, m_VBO;
    unsigned int m_shaderProgram;
    int m_screenWidth, m_screenHeight;

    // Встроенный растровый шрифт (8x8 пикселей на символ)
    static const unsigned char m_fontData[95][8];  // 95 символов (32-126)

    unsigned int m_textureAtlas;

    void setupShader();
    void buildTextureAtlas();
    int charToIndex(char c) const;

public:
    TextRenderer();
    ~TextRenderer();

    bool init(int screenWidth, int screenHeight);
    void renderText(const std::string& text, float x, float y, float scale, const Color& color);
    void cleanup();
};