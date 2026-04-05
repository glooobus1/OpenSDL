#pragma once
#include "../vec3.hpp"
#include "../color.hpp"
#include "../vec2.hpp"
#include <vector>
#include <GL/glew.h>

struct ObjMesh {
    std::vector<vec3> vertices;
    std::vector<vec3> normals;
    std::vector<vec2> uvs;
    std::vector<Color> vertexColors;
    std::vector<unsigned int> indices;

    bool hasNormals = false;
    bool hasUVs = false;
    bool hasColors = false;
    static bool s_shadowPass;

    static void beginShadowPass() { s_shadowPass = true; }
    static void endShadowPass() { s_shadowPass = false; }

    void draw() const {
        if (vertices.empty()) return;

        glBegin(GL_TRIANGLES);
        for (size_t i = 0; i < indices.size(); i += 3) {
            for (int j = 0; j < 3; j++) {
                int idx = indices[i + j];

                if (hasNormals && !normals.empty()) {
                    glNormal3f(normals[idx].x, normals[idx].y, normals[idx].z);
                }

                // ТОЛЬКО если НЕ в режиме тени — используем цвета объектов
                if (!s_shadowPass && hasColors && !vertexColors.empty()) {
                    const Color& c = vertexColors[idx];
                    glColor4f(c.r, c.g, c.b, c.a);
                }

                if (hasUVs && !uvs.empty()) {
                    glTexCoord2f(uvs[idx].x, uvs[idx].y);
                }

                glVertex3f(vertices[idx].x, vertices[idx].y, vertices[idx].z);
            }
        }
        glEnd();
    }

    void clear() {
        vertices.clear();
        normals.clear();
        uvs.clear();
        vertexColors.clear();
        indices.clear();
    }
};