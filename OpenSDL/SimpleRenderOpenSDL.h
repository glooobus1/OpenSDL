#pragma once
// =============================
//
// By Glooobus
//
// =============================

// Математика
#include "vec2.hpp"
#include "vec3.hpp"  
#include "vec4.hpp"
#include "mat4.hpp"
#include "quat.hpp"
#include "color.hpp"

// Трансформации
#include "transform.hpp"

// Графика
#include "camera.hpp"
#include "shader.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "Sun.hpp"

// Рендерер
#include "StandartOpenGlRender.hpp"

// Стандартные объекты / меши
#include "Cube.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "ObjMesh.hpp"
#include "ObjLoader.hpp"

// Системы
#include "InputSystem.hpp"
#include "SimpleSDL.hpp"

// Библиотеки
#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

// =============================
// Утилиты для создания объектов
// =============================

inline Cube* CreateCube(StandartOpenGlRender& renderer, const vec3& position, float size = 1.0f) {
    Cube* cube = renderer.addObject<Cube>();
    cube->transform.position = position;
    cube->transform.scale = vec3(size, size, size);
    return cube;
}

inline Sphere* CreateSphere(StandartOpenGlRender& renderer, const vec3& position,
                            float radius = 1.0f, int segments = 16) {
    Sphere* sphere = renderer.addObject<Sphere>(radius, segments);
    sphere->transform.position = position;
    return sphere;
}

inline Plane* CreatePlane(StandartOpenGlRender& renderer, const vec3& position,
                          float size = 2.0f) {
    Plane* plane = renderer.addObject<Plane>(size);
    plane->transform.position = position;
    return plane;
}

inline void CreateCubeGrid(StandartOpenGlRender& renderer, int rows, int cols,
                           float spacing = 2.0f, float size = 1.0f) {
    for (int z = 0; z < rows; ++z) {
        for (int x = 0; x < cols; ++x) {
            vec3 pos(
                (x - cols / 2) * spacing,
                0.0f,
                (z - rows / 2) * spacing
            );
            CreateCube(renderer, pos, size);
        }
    }
}

// Объект для отображения произвольной OBJ-модели
class ObjModelObject : public SceneObject {
public:
    ObjMesh mesh;

    explicit ObjModelObject(const ObjMesh& srcMesh) : mesh(srcMesh) {}

    void render() override {
        mesh.draw();
    }
};

inline ObjModelObject* LoadObjModel(StandartOpenGlRender& renderer,
                                    const std::string& filename,
                                    const vec3& position) {
    ObjMesh mesh;
    if (!ObjLoader::load(filename, mesh)) {
        std::cout << "Failed to load OBJ model: " << filename << std::endl;
        return nullptr;
    }

    ObjModelObject* obj = renderer.addObject<ObjModelObject>(mesh);
    obj->transform.position = position;
    return obj;
}