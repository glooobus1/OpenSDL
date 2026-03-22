#pragma once
#include "StandartOpenGlRender.hpp"
#include "ObjMesh.hpp"
#include <GL/glew.h>

class Cube : public SceneObject {
private:
    ObjMesh mesh;

public:
    Cube() {
        // Вершины куба
        mesh.vertices = {
            vec3(-1, -1, -1), vec3(1, -1, -1), vec3(1,  1, -1), vec3(-1,  1, -1),
            vec3(-1, -1,  1), vec3(1, -1,  1), vec3(1,  1,  1), vec3(-1,  1,  1)
        };

        // Одинаковый цвет для всех вершин (коричневый, как дерево)
        Color c = Color::fromRGB(139, 69, 19); // saddle brown
        mesh.vertexColors = { c, c, c, c, c, c, c, c };

        // Индексы треугольников (12 треугольников)
        int triangles[12][3] = {
            {1, 2, 6}, {1, 6, 5},  // правая
            {0, 4, 7}, {0, 7, 3},  // левая
            {3, 7, 6}, {3, 6, 2},  // верхняя
            {0, 1, 5}, {0, 5, 4},  // нижняя
            {4, 5, 6}, {4, 6, 7},  // передняя
            {0, 3, 2}, {0, 2, 1}   // задняя
        };

        for (int i = 0; i < 12; i++) {
            mesh.indices.push_back(triangles[i][0]);
            mesh.indices.push_back(triangles[i][1]);
            mesh.indices.push_back(triangles[i][2]);
        }

        mesh.hasColors = true;
    }

    void render() override {
        mesh.draw();
    }

    void update(float /*deltaTime*/) override {
        // Без автоматического вращения
    }
};