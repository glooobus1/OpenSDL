#pragma once
#include "StandartOpenGlRender.hpp"

class WhiteSphere {
public:
    static void renderAroundPlayer(const vec3& playerPos, float radius, int segments = 32) {
        glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);

        // Отключаем всё, что может помешать
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Сохраняем матрицу
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        // Перемещаемся в позицию игрока
        glTranslatef(playerPos.x, playerPos.y, playerPos.z);

        // Рисуем сферу (белый цвет, не зависит от освещения)
        glColor4f(1.0f, 1.0f, 1.0f, 0.3f);  // полупрозрачный белый

        // Рисуем сферу через треугольные полосы
        for (int i = 0; i <= segments; i++) {
            float theta = i * 3.14159f / segments;
            float y = cos(theta) * radius;
            float r = sin(theta) * radius;

            glBegin(GL_TRIANGLE_STRIP);
            for (int j = 0; j <= segments; j++) {
                float phi = j * 2 * 3.14159f / segments;
                float x = r * cos(phi);
                float z = r * sin(phi);

                glVertex3f(x, y, z);

                float y2 = cos(theta + 3.14159f / segments) * radius;
                float r2 = sin(theta + 3.14159f / segments) * radius;
                float x2 = r2 * cos(phi);
                float z2 = r2 * sin(phi);
                glVertex3f(x2, y2, z2);
            }
            glEnd();
        }

        // Внутренняя сфера (ярче)
        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
        for (int i = 0; i <= segments; i++) {
            float theta = i * 3.14159f / segments;
            float y = cos(theta) * radius * 0.6f;
            float r = sin(theta) * radius * 0.6f;

            glBegin(GL_TRIANGLE_STRIP);
            for (int j = 0; j <= segments; j++) {
                float phi = j * 2 * 3.14159f / segments;
                float x = r * cos(phi);
                float z = r * sin(phi);

                glVertex3f(x, y, z);

                float y2 = cos(theta + 3.14159f / segments) * radius * 0.6f;
                float r2 = sin(theta + 3.14159f / segments) * radius * 0.6f;
                float x2 = r2 * cos(phi);
                float z2 = r2 * sin(phi);
                glVertex3f(x2, y2, z2);
            }
            glEnd();
        }

        glPopMatrix();
        glPopAttrib();
    }
};