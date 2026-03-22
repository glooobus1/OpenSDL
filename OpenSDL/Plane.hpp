#pragma once
#include "StandartOpenGlRender.hpp"
#include "ObjMesh.hpp"

class Plane : public SceneObject {
private:
    ObjMesh mesh;

public:
    Plane(float size = 2.0f) {
        float s = size / 2;

        // Вершины плоскости
        mesh.vertices = {
            vec3(-s, 0, -s),  // 0
            vec3(s, 0, -s),  // 1
            vec3(s, 0,  s),  // 2
            vec3(-s, 0,  s)   // 3
        };

        // Цвета (одинаковый зелёный, как трава)
        Color grass = Color::fromRGB(50, 160, 60);
        mesh.vertexColors = { grass, grass, grass, grass };

        // Текстурные координаты
        mesh.uvs = {
            vec2(0, 0),
            vec2(1, 0),
            vec2(1, 1),
            vec2(0, 1)
        };

        // Два треугольника
        mesh.indices = { 0, 1, 2, 2, 3, 0 };

        mesh.hasColors = true;
        mesh.hasUVs = true;
    }

    void render() override {
        mesh.draw();
    }
};