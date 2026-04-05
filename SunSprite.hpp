#pragma once
#include "StandartOpenGlRender.hpp"

class SunSprite : public SceneObject {
private:
    unsigned int textureID;
    float angle;
    float radius;

public:
    SunSprite() : angle(0), radius(0.1f) {
        // Создаём текстуру солнца (жёлтый круг)
        int size = 64;
        std::vector<unsigned char> pixels(size * size * 4);

        int center = size / 2;
        int radius_px = size / 3;

        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                int dx = x - center;
                int dy = y - center;
                float dist = sqrt(dx * dx + dy * dy);
                int idx = (y * size + x) * 4;

                if (dist < radius_px) {
                    // Солнце (жёлтое)
                    pixels[idx + 0] = 255;
                    pixels[idx + 1] = 220;
                    pixels[idx + 2] = 100;
                    pixels[idx + 3] = 255;
                }
                else if (dist < radius_px + 10) {
                    // Свечение (полупрозрачное)
                    float alpha = 1.0f - (dist - radius_px) / 10.0f;
                    pixels[idx + 0] = 255;
                    pixels[idx + 1] = 200;
                    pixels[idx + 2] = 50;
                    pixels[idx + 3] = (unsigned char)(alpha * 128);
                }
                else {
                    pixels[idx + 0] = 0;
                    pixels[idx + 1] = 0;
                    pixels[idx + 2] = 0;
                    pixels[idx + 3] = 0;
                }
            }
        }

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    ~SunSprite() {
        glDeleteTextures(1, &textureID);
    }

    void render() override {
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, 1, 0, 1, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Позиция солнца (в правом верхнем углу)
        float sunX = 0.85f;
        float sunY = 0.85f;
        float sunSize = 0.12f;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(sunX - sunSize, sunY - sunSize);
        glTexCoord2f(1, 0); glVertex2f(sunX + sunSize, sunY - sunSize);
        glTexCoord2f(1, 1); glVertex2f(sunX + sunSize, sunY + sunSize);
        glTexCoord2f(0, 1); glVertex2f(sunX - sunSize, sunY + sunSize);
        glEnd();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
    }

    void update(float dt) override {}
};