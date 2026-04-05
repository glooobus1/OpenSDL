#pragma once
#include "StandartOpenGlRender.hpp"

class SimpleSky : public SceneObject {
private:
    unsigned int textureID;
    int width, height;

public:
    SimpleSky() {
        // Создаём текстуру градиента
        width = 512;
        height = 512;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Создаём градиентные данные
        std::vector<unsigned char> pixels(width * height * 3);

        for (int y = 0; y < height; y++) {
            float t = (float)y / height;  // 0 внизу, 1 вверху
            // Синий цвет сверху, светлый снизу
            unsigned char r = (unsigned char)(80 + 100 * t);     // красный компонент
            unsigned char g = (unsigned char)(120 + 100 * t);    // зелёный компонент
            unsigned char b = (unsigned char)(200 + 55 * t);     // синий компонент

            for (int x = 0; x < width; x++) {
                int idx = (y * width + x) * 3;
                pixels[idx + 0] = r;
                pixels[idx + 1] = g;
                pixels[idx + 2] = b;
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    ~SimpleSky() {
        glDeleteTextures(1, &textureID);
    }

    void render() override {
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, 1, 0, 1, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(0, 0);
        glTexCoord2f(1, 0); glVertex2f(1, 0);
        glTexCoord2f(1, 1); glVertex2f(1, 1);
        glTexCoord2f(0, 1); glVertex2f(0, 1);
        glEnd();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        glDisable(GL_TEXTURE_2D);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
    }

    void update(float dt) override {}
};