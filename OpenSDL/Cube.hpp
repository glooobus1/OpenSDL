#pragma once
#include "SceneObject.hpp"
#include "Mesh.hpp"

class Cube : public SceneObject {
private:
    Mesh mesh;

public:
    Cube() {
        std::vector<vec3> vertices = {
            vec3(-1, -1, -1), vec3(1, -1, -1), vec3(1,  1, -1), vec3(-1,  1, -1),
            vec3(-1, -1,  1), vec3(1, -1,  1), vec3(1,  1,  1), vec3(-1,  1,  1)
        };

        std::vector<vec3> normals = {
            vec3(-1,-1,-1).normalize(), vec3(1,-1,-1).normalize(),
            vec3(1, 1,-1).normalize(), vec3(-1, 1,-1).normalize(),
            vec3(-1,-1, 1).normalize(), vec3(1,-1, 1).normalize(),
            vec3(1, 1, 1).normalize(), vec3(-1, 1, 1).normalize()
        };

        Color brown = Color::fromRGB(139, 69, 19);
        std::vector<Color> colors(8, brown);

        std::vector<unsigned int> indices = {
            1,2,6, 1,6,5, 0,4,7, 0,7,3,
            3,7,6, 3,6,2, 0,1,5, 0,5,4,
            4,5,6, 4,6,7, 0,3,2, 0,2,1
        };

        mesh.uploadData(vertices, normals, {}, colors, indices);
    }

    void render() override { mesh.draw(); }

    void setColor(const Color& color) {
        std::vector<vec3> vertices = {
            vec3(-1, -1, -1), vec3(1, -1, -1), vec3(1,  1, -1), vec3(-1,  1, -1),
            vec3(-1, -1,  1), vec3(1, -1,  1), vec3(1,  1,  1), vec3(-1,  1,  1)
        };
        std::vector<vec3> normals = {
            vec3(-1,-1,-1).normalize(), vec3(1,-1,-1).normalize(),
            vec3(1, 1,-1).normalize(), vec3(-1, 1,-1).normalize(),
            vec3(-1,-1, 1).normalize(), vec3(1,-1, 1).normalize(),
            vec3(1, 1, 1).normalize(), vec3(-1, 1, 1).normalize()
        };
        std::vector<Color> colors(8, color);
        std::vector<unsigned int> indices = {
            1,2,6, 1,6,5, 0,4,7, 0,7,3,
            3,7,6, 3,6,2, 0,1,5, 0,5,4,
            4,5,6, 4,6,7, 0,3,2, 0,2,1
        };
        mesh.uploadData(vertices, normals, {}, colors, indices);
    }
};