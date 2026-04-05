#pragma once
#include "StandartOpenGlRender.hpp"
#include "Mesh.hpp"
#include <cmath>

class Sphere : public SceneObject {
private:
    Mesh mesh;
    float radius;
    int segments;

public:
    Sphere(float r = 1.0f, int seg = 16) : radius(r), segments(seg) {
        generateSphere();
    }

    void render() override { mesh.draw(); }

    void setColor(const Color& color) {
        generateSphereWithColor(color);
    }

private:
    void generateSphere() {
        Color green = Color::fromRGB(34, 139, 34);
        generateSphereWithColor(green);
    }

    void generateSphereWithColor(const Color& color) {
        std::vector<vec3> vertices;
        std::vector<vec3> normals;
        std::vector<Color> colors;
        std::vector<unsigned int> indices;

        for (int i = 0; i <= segments; i++) {
            float theta = i * 3.14159f / segments;
            for (int j = 0; j <= segments; j++) {
                float phi = j * 2 * 3.14159f / segments;

                float x = radius * sin(theta) * cos(phi);
                float y = radius * cos(theta);
                float z = radius * sin(theta) * sin(phi);

                vertices.push_back(vec3(x, y, z));
                normals.push_back(vec3(x, y, z).normalize());
                colors.push_back(color);
            }
        }

        for (int i = 0; i < segments; i++) {
            for (int j = 0; j < segments; j++) {
                int a = i * (segments + 1) + j;
                int b = i * (segments + 1) + j + 1;
                int c = (i + 1) * (segments + 1) + j;
                int d = (i + 1) * (segments + 1) + j + 1;

                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);

                indices.push_back(b);
                indices.push_back(d);
                indices.push_back(c);
            }
        }

        mesh.uploadData(vertices, normals, {}, colors, indices);
    }
};