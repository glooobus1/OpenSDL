#pragma once
#include "vec3.hpp"
#include "vec2.hpp"
#include "color.hpp"
#include <vector>
#include <GL/glew.h>

class Mesh {
private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
    bool hasNormals;
    bool hasUVs;
    bool hasColors;

public:
    Mesh() : VAO(0), VBO(0), EBO(0), indexCount(0),
        hasNormals(false), hasUVs(false), hasColors(false) {
    }

    ~Mesh() { clear(); }

    void uploadData(
        const std::vector<vec3>& vertices,
        const std::vector<vec3>& normals,
        const std::vector<vec2>& uvs,
        const std::vector<Color>& colors,
        const std::vector<unsigned int>& indices
    ) {
        clear();

        indexCount = (unsigned int)indices.size();
        hasNormals = !normals.empty();
        hasUVs = !uvs.empty();
        hasColors = !colors.empty();

        // Подготовка interleaved данных
        std::vector<float> interleavedData;

        for (size_t i = 0; i < vertices.size(); i++) {
            // Позиция (3 floats)
            interleavedData.push_back(vertices[i].x);
            interleavedData.push_back(vertices[i].y);
            interleavedData.push_back(vertices[i].z);

            // Нормаль (3 floats)
            if (hasNormals && i < normals.size()) {
                interleavedData.push_back(normals[i].x);
                interleavedData.push_back(normals[i].y);
                interleavedData.push_back(normals[i].z);
            }

            // Цвет (4 floats)
            if (hasColors && i < colors.size()) {
                interleavedData.push_back(colors[i].r);
                interleavedData.push_back(colors[i].g);
                interleavedData.push_back(colors[i].b);
                interleavedData.push_back(colors[i].a);
            }

            // UV (2 floats)
            if (hasUVs && i < uvs.size()) {
                interleavedData.push_back(uvs[i].x);
                interleavedData.push_back(uvs[i].y);
            }
        }

        // Вычисление stride
        unsigned int stride = 3 * sizeof(float); // позиция
        if (hasNormals) stride += 3 * sizeof(float);
        if (hasColors) stride += 4 * sizeof(float);
        if (hasUVs) stride += 2 * sizeof(float);

        // Создание VAO, VBO, EBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(float),
            interleavedData.data(), GL_STATIC_DRAW);

        // EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
            indices.data(), GL_STATIC_DRAW);

        // Атрибуты
        unsigned int offset = 0;

        // Позиция (location = 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
        glEnableVertexAttribArray(0);
        offset += 3 * sizeof(float);

        // Нормаль (location = 1)
        if (hasNormals) {
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
            glEnableVertexAttribArray(1);
            offset += 3 * sizeof(float);
        }

        // Цвет (location = 2)
        if (hasColors) {
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)offset);
            glEnableVertexAttribArray(2);
            offset += 4 * sizeof(float);
        }

        // UV (location = 3)
        if (hasUVs) {
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);
            glEnableVertexAttribArray(3);
        }

        glBindVertexArray(0);
    }

    void draw() const {
        if (VAO == 0 || indexCount == 0) return;
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void clear() {
        if (EBO) { glDeleteBuffers(1, &EBO); EBO = 0; }
        if (VBO) { glDeleteBuffers(1, &VBO); VBO = 0; }
        if (VAO) { glDeleteVertexArrays(1, &VAO); VAO = 0; }
        indexCount = 0;
    }
};