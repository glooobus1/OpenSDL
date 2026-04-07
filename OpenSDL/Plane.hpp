#pragma once
#include "SceneObject.hpp"
#include "Mesh.hpp"

class Plane : public SceneObject {
private:
    Mesh mesh;

public:
    Plane(float size = 2.0f) {
        float s = size / 2;

        std::vector<vec3> vertices = {
            vec3(-s, -1, -s),
            vec3(s, -1, -s),
            vec3(s, -1,  s),
            vec3(-s, -1,  s)
        };

        std::vector<vec3> normals = {
            vec3(0, 1, 0), vec3(0, 1, 0), vec3(0, 1, 0), vec3(0, 1, 0)
        };

        Color grass = Color::fromRGB(50, 160, 60);
        std::vector<Color> colors = { grass, grass, grass, grass };

        std::vector<vec2> uvs = {
            vec2(0, 0), vec2(1, 0), vec2(1, 1), vec2(0, 1)
        };

        std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

        mesh.uploadData(vertices, normals, uvs, colors, indices);
    }

    void render() override { mesh.draw(); }
};