#pragma once
#include "StandartOpenGlRender.hpp"

class SkyTexture {
private:
    unsigned int textureID;
    int width, height;

public:
    SkyTexture() {
        createGradientTexture();
    }
    void render() {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, 1, 0, 1, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // ===== ТЕКСТУРА НЕБА =====
        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(0, 0);
        glTexCoord2f(1, 0); glVertex2f(1, 0);
        glTexCoord2f(1, 1); glVertex2f(1, 1);
        glTexCoord2f(0, 1); glVertex2f(0, 1);
        glEnd();

        // ===== СОЛНЦЕ (ВСЕГДА ВВЕРХУ) =====
        glDisable(GL_TEXTURE_2D);

        float sunX = 0.85f;
        float sunY = 0.92f;  // высоко, над горизонтом
        float sunRadius = 0.09f;

        // Свечение
        glColor4f(1.0f, 0.8f, 0.3f, 0.4f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(sunX, sunY);
        for (int i = 0; i <= 30; i++) {
            float angle = i * 3.14159f * 2 / 30;
            float r = sunRadius * 2.2f;
            glVertex2f(sunX + cos(angle) * r, sunY + sin(angle) * r);
        }
        glEnd();

        // Само солнце
        glColor3f(1.0f, 0.9f, 0.3f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(sunX, sunY);
        for (int i = 0; i <= 30; i++) {
            float angle = i * 3.14159f * 2 / 30;
            glVertex2f(sunX + cos(angle) * sunRadius, sunY + sin(angle) * sunRadius);
        }
        glEnd();

        // Блик
        glColor4f(1.0f, 1.0f, 0.9f, 0.7f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(sunX - 0.015f, sunY + 0.015f);
        for (int i = 0; i <= 20; i++) {
            float angle = i * 3.14159f * 2 / 20;
            float r = sunRadius * 0.35f;
            glVertex2f(sunX - 0.015f + cos(angle) * r, sunY + 0.015f + sin(angle) * r);
        }
        glEnd();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
    }

private:
    void createGradientTexture() {
        width = 512;
        height = 512;

        std::vector<unsigned char> pixels(width * height * 3);

        for (int y = 0; y < height; y++) {
            float t = (float)y / height;

            unsigned char r, g, b;

            if (t < 0.6f) {
                float tt = t / 0.6f;
                r = (unsigned char)(20 + 100 * tt);
                g = (unsigned char)(60 + 100 * tt);
                b = (unsigned char)(180 + 75 * tt);
            }
            else {
                float tt = (t - 0.6f) / 0.4f;
                r = (unsigned char)(120 + 135 * tt);
                g = (unsigned char)(160 + 95 * tt);
                b = (unsigned char)(255 - 100 * tt);
            }

            for (int x = 0; x < width; x++) {
                int idx = (y * width + x) * 3;
                pixels[idx + 0] = r;
                pixels[idx + 1] = g;
                pixels[idx + 2] = b;
            }
        }

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
};