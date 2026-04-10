#include "SimpleRenderOpenSDL.h"
#include "StandartPlayerController.hpp"
#include "Cube.hpp"
#include "Plane.hpp"
#include "Sun.hpp"
#include "SoundSystem.hpp"
#include "Camera.hpp"

// Глобальные переменные
StandartOpenGlRender* g_render = nullptr;
StandartPlayerController* g_playerController = nullptr;

// Функция обновления
void Update(float dt) {
    if (!g_render || !g_playerController) return;

    const bool* keys = SDL_GetKeyboardState(nullptr);
    g_playerController->update(dt, keys);

    std::string title = "Test Flight | FPS: " + std::to_string(g_render->getFPS());
    g_render->setWindowTitle(title);
}

int main() {
    // ===== ИНИЦИАЛИЗАЦИЯ РЕНДЕРА =====
    StandartOpenGlRender render;
    g_render = &render;

    if (!render.init(800, 600, "Test Flight - Physics Demo")) {
        return -1;
    }

    render.setUpdateCallback(Update);
    PhysicsWorld& physics = render.getPhysicsWorld();
    // ===== КАМЕРА И КОНТРОЛЛЕР =====
    Camera renderCamera;
    renderCamera.setPosition(vec3(0, 3, 8));
    renderCamera.lookAt(vec3(0, 0, 0));

    render.setCamera(&renderCamera);

    StandartPlayerController playerController;
    playerController.init(&renderCamera, &physics, render.getWindow(), 800, 600);
    playerController.setMouseLocked(true);
    playerController.setMoveSpeed(8.0f);
    g_playerController = &playerController;

  

    // ===== ПОЛ =====
    Plane* ground = CreatePlane(render, vec3(0, -1.5f, 0), 20.0f);
    ground->transform.scale = vec3(15, 1, 15);
    ground->rigidbody.setIsStatic(true);
    ground->setColliderSize(vec3(15.0f, 0.1f, 15.0f));
    physics.addObject(ground);

    // Создаём 100x100 сетку кубов (10 000 штук)
    int gridSize = 9;
    float spacing = 3;

    for (int x = -gridSize / 2; x < gridSize / 2; x++) {
        for (int z = -gridSize / 2; z < gridSize / 2; z++) {
            Cube* cube = CreateCube(render, vec3(x * spacing, 5, z * spacing), 1.0f);
            cube->setColor(Color::fromHEX(0x88AAFF));
            cube->setColliderSize(vec3(1.0f, 1.0f, 1.0f));
            cube->rigidbody.setIsStatic(false);     // не статичный
            cube->rigidbody.setUseGravity(true);    // гравитация включена
            cube->rigidbody.setMass(1.0f);          // масса 1 кг
            cube->rigidbody.setRestitution(0.5f);   // упругость
            physics.addObject(cube);                // добавить в физический мир
        }
    }

    // ===== СОЛНЦЕ =====
    vec3 sunPos = vec3(20, 15, 5);
    // Солнце (источник света)
    Sun* sun = render.addObject<Sun>(sunPos, vec3(1.0f, 1.0f, 1.0f), 1.2f);
    sun->rigidbody.setIsStatic(true);
    physics.addObject(sun);
    // Сфера (видимый шар на том же месте)
    Sphere* sunSphere = render.addObject<Sphere>(2.0f, 32);  // радиус 2, сегменты 32
    sunSphere->transform.position = sunPos;  // ← ТА ЖЕ ПОЗИЦИЯ, ЧТО И У СОЛНЦА
    sunSphere->setColor(Color::fromHEX(0xFFAA33));
    sunSphere->rigidbody.setIsStatic(true);

    // ===== МУЗЫКА =====
    SoundSystem audio;
    if (audio.init()) {
        std::string soundPath = audio.setPath("saunds\\edge\\StartEdge.wav");
        if (audio.loadSound("background", soundPath)) {
            std::cout << "🎵 Музыка загружена!" << std::endl;
            audio.play("background", false, 0.7f);
        }
        else {
            std::cout << "Файл не найден: " << soundPath << std::endl;
        }
    }

    // ===== ЗАПУСК =====
    std::cout << "\n=== Управление ===" << std::endl;
    std::cout << "WASD - движение | Мышь - поворот | Пробел - прыжок" << std::endl;
    std::cout << "ESC - отпустить мышь | ПКМ - захватить мышь" << std::endl;

    render.run();

    return 0;
}