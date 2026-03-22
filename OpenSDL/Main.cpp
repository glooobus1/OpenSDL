#include "SimpleRenderOpenSDL.h"

int main() {
    StandartOpenGlRender render;

    if (!render.init(800, 600, "3D Engine")) {
        return -1;
    }

    // Демо-сцена:
    // 1) большая зелёная плоскость как земля
    Plane* ground = CreatePlane(render, vec3(0, -1, 0), 40.0f);
    ground->transform.scale = vec3(20, 1, 20);

    // 2) "дерево": ствол + крона (увеличенные)
    Cube* trunk = CreateCube(render, vec3(0, 0, 0), 1.0f);
    trunk->transform.scale = vec3(0.6f, 2.5f, 0.6f);

    Sphere* crown = CreateSphere(render, vec3(0, 3.5f, 0), 2.0f, 32);

    // 3) ещё несколько объектов для ориентира
    CreateCube(render, vec3(5, 0, 5), 1.0f);
    CreateCube(render, vec3(-5, 0, 3), 1.5f);

    render.run();
    return 0;
}