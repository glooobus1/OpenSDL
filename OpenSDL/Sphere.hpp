#pragma once
#include "StandartOpenGlRender.hpp"
#include "ObjMesh.hpp"
#include <cmath>

class Sphere : public SceneObject {
private:
    ObjMesh mesh;
    int segments;

public:
    Sphere(float radius = 1.0f, int segments = 16) : segments(segments) {
        generateSphere(radius, segments);
    }

    void render() override {
        mesh.draw();
    }

private:
    void generateSphere(float radius, int segments) {
        mesh.vertices.clear();
        mesh.indices.clear();

        // Вершины и однотонный цвет (зелёная крона)
        Color c = Color::fromRGB(34, 139, 34); // forest green
        for (int i = 0; i <= segments; i++) {
            float theta = i * 3.14159f / segments;  // 0 to pi

            for (int j = 0; j <= segments; j++) {
                float phi = j * 2 * 3.14159f / segments;  // 0 to 2pi

                float x = radius * sin(theta) * cos(phi);
                float y = radius * cos(theta);
                float z = radius * sin(theta) * sin(phi);

                mesh.vertices.push_back(vec3(x, y, z));
                mesh.vertexColors.push_back(c);
            }
        }

        // Индексы
        for (int i = 0; i < segments; i++) {
            for (int j = 0; j < segments; j++) {
                int a = i * (segments + 1) + j;
                int b = i * (segments + 1) + j + 1;
                int c = (i + 1) * (segments + 1) + j;
                int d = (i + 1) * (segments + 1) + j + 1;

                mesh.indices.push_back(a);
                mesh.indices.push_back(b);
                mesh.indices.push_back(c);

                mesh.indices.push_back(b);
                mesh.indices.push_back(d);
                mesh.indices.push_back(c);
            }
        }

        mesh.hasColors = true;
    }
};