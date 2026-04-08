#include "SimpleRenderOpenSDL.h"
#include "CameraObject.hpp"
#include "Cube.hpp"
#include "Plane.hpp"
#include "Sun.hpp"
#include "SoundSystem.hpp"

// Глобальные переменные
StandartOpenGlRender* g_render = nullptr;
CameraObject* playerCamera = nullptr;

void Update(float dt) {
    const bool* keys = SDL_GetKeyboardState(nullptr);

    float speed = 15.0f;

    if (keys[SDL_SCANCODE_W]) {
        g_render->getCamera().move(g_render->getCamera().forward() * speed * dt);
    }
    if (keys[SDL_SCANCODE_S]) {
        g_render->getCamera().move(-g_render->getCamera().forward() * speed * dt);
    }
    if (keys[SDL_SCANCODE_A]) {
        g_render->getCamera().move(-g_render->getCamera().right() * speed * dt);
    }
    if (keys[SDL_SCANCODE_D]) {
        g_render->getCamera().move(g_render->getCamera().right() * speed * dt);
    }

    if (g_render) {
        std::string title = "3D Engine | FPS: " + std::to_string(g_render->getFPS());
        g_render->setWindowTitle(title);
    }
}

int main() {
    // ===== СНАЧАЛА ИНИЦИАЛИЗИРУЕМ РЕНДЕР =====
    StandartOpenGlRender render;
    g_render = &render;

    if (!render.init(800, 600, "3D Engine - 5 Cubes")) {
        return -1;
    }

    render.setUpdateCallback(Update);

    PhysicsWorld& physics = render.getPhysicsWorld();

    // ===== КАМЕРА =====
    playerCamera = render.addObject<CameraObject>();
    playerCamera->init(render.getWindow());
    playerCamera->setPosition(vec3(0, 3, 8));
    playerCamera->setMouseLocked(true);

    // ===== СОЛНЦЕ (свет) =====
    Sun* sun = render.addObject<Sun>(vec3(5, 15, 5), vec3(1.0f, 1.0f, 1.0f), 1.2f);
    sun->rigidbody.setIsStatic(true);
    physics.addObject(sun);

    // ===== ЗЕМЛЯ =====
    Plane* ground = CreatePlane(render, vec3(0, -1, 0), 100.0f);
    ground->transform.scale = vec3(20, 1, 20);
    ground->rigidbody.setIsStatic(true);
    ground->setColliderSize(vec3(20.0f, 0.1f, 20.0f));
    physics.addObject(ground);

    // ===== 5 КУБОВ =====
    Cube* cube1 = CreateCube(render, vec3(0, 0, 0), 1.0f);
    cube1->setColor(Color::red());
    cube1->rigidbody.setIsStatic(true);
    physics.addObject(cube1);

    Cube* cube2 = CreateCube(render, vec3(-2, 0, 0), 1.0f);
    cube2->setColor(Color::green());
    cube2->rigidbody.setIsStatic(true);
    physics.addObject(cube2);

    Cube* cube3 = CreateCube(render, vec3(2, 0, 0), 1.0f);
    cube3->setColor(Color::blue());
    cube3->rigidbody.setIsStatic(true);
    physics.addObject(cube3);

    Cube* cube4 = CreateCube(render, vec3(0, 2, 0), 1.0f);
    cube4->setColor(Color::yellow());
    cube4->rigidbody.setIsStatic(true);
    physics.addObject(cube4);

    Cube* cube5 = CreateCube(render, vec3(0, 4, 0), 1.0f);
    cube5->setColor(Color::magenta());
    cube5->rigidbody.setIsStatic(true);
    physics.addObject(cube5);

    // ===== ЗВУК (ПОСЛЕ ИНИЦИАЛИЗАЦИИ РЕНДЕРА) =====
    SoundSystem audio;
    if (audio.init()) {
        audio.loadSound("test", audio.setPath("saunds\\edge\\StartEdge.wav"));
        std::cout << "Воспроизведение звука..." << std::endl;
        int id = audio.play("test", false, 1.0f);

        // Не ждём здесь - звук будет играть в фоне
        // audio.update() будет вызываться в игровом цикле, если нужно

        // Если нужно ждать окончания, то делаем это в отдельном потоке или проверяем в Update
    }
    else {
        std::cerr << "Ошибка инициализации звука!" << std::endl;
    }

    // ===== ЗАПУСК ИГРОВОГО ЦИКЛА =====
    render.run();

    return 0;
}