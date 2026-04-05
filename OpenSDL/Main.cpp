#include "SimpleRenderOpenSDL.h"
#include "CameraObject.hpp"
#include "Cube.hpp"
#include "Plane.hpp"
#include "Sun.hpp"
#include "InputController.hpp"

StandartOpenGlRender* g_render = nullptr;
CameraObject* playerCamera = nullptr;
InputController gamepad;

// Камера
float yaw = 0.0f;
float pitch = 0.0f;

// Deadzone
float deadzone(float v, float dz = 0.15f) {
    return (fabs(v) < dz) ? 0.0f : v;
}

void Update(float dt) {
    // ❗ ОБЯЗАТЕЛЬНО
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            exit(0);
        }
    }

    gamepad.update();

    const bool* keys = SDL_GetKeyboardState(nullptr);

    // ===== ГЕЙМПАД =====
    if (gamepad.isConnected()) {

        // --- ДВИЖЕНИЕ (левый стик) ---
        float lx = deadzone(gamepad.getAxis(InputController::LeftX));
        float ly = deadzone(gamepad.getAxis(InputController::LeftY));

        vec3 forward = g_render->getCamera().forward();
        vec3 right = g_render->getCamera().right();

        forward.y = 0;
        right.y = 0;

        forward = forward.normalize();
        right = right.normalize();

        vec3 moveDir = forward * ly + right * lx;

        if (moveDir.length() > 1.0f)
            moveDir = moveDir.normalize();

        if (moveDir.length() > 0.0f) {
            g_render->getCamera().move(moveDir * 5.0f * dt);
        }

        // --- КАМЕРА (правый стик) ---
        float rx = deadzone(gamepad.getAxis(InputController::RightX));
        float ry = deadzone(gamepad.getAxis(InputController::RightY));

        float sensitivity = 2.5f;

        yaw -= rx * sensitivity * dt;
        pitch -= ry * sensitivity * dt;

        // Ограничение вверх/вниз
        if (pitch > 1.3f) pitch = 1.3f;
        if (pitch < -1.3f) pitch = -1.3f;

        quat qPitch = quat::fromAxisAngle(vec3(1, 0, 0), pitch);
        quat qYaw = quat::fromAxisAngle(vec3(0, 1, 0), yaw);

        g_render->getCamera().setRotation(qYaw * qPitch);

        // --- ЗУМ ---
        float lt = gamepad.getAxis(InputController::LeftTrigger);
        float rt = gamepad.getAxis(InputController::RightTrigger);

        if (lt > 0.1f) {
            g_render->getCamera().setFov(40.0f);
        }
        else if (rt > 0.1f) {
            g_render->getCamera().setFov(80.0f);
        }
        else {
            g_render->getCamera().setFov(60.0f);
        }
    }

    // ===== КЛАВИАТУРА =====
    if (keys[SDL_SCANCODE_W])
        g_render->getCamera().move(g_render->getCamera().forward() * 5.0f * dt);

    if (keys[SDL_SCANCODE_S])
        g_render->getCamera().move(-g_render->getCamera().forward() * 5.0f * dt);

    if (keys[SDL_SCANCODE_A])
        g_render->getCamera().move(-g_render->getCamera().right() * 5.0f * dt);

    if (keys[SDL_SCANCODE_D])
        g_render->getCamera().move(g_render->getCamera().right() * 5.0f * dt);
}

int main() {
    StandartOpenGlRender render;
    g_render = &render;

    if (!render.init(800, 600, "3D Engine")) {
        return -1;
    }

    // 🎮 геймпад
    gamepad.init();

    render.setUpdateCallback(Update);

    PhysicsWorld& physics = render.getPhysicsWorld();

    // ===== КАМЕРА =====
    playerCamera = render.addObject<CameraObject>();
    playerCamera->init(render.getWindow());
    playerCamera->setPosition(vec3(0, 1.6f, 5));
    playerCamera->setMouseLocked(true);

    // ===== СВЕТ =====
    Sun* sun = render.addObject<Sun>(
        vec3(5, 15, 5),
        vec3(1.0f, 1.0f, 1.0f),
        1.2f
    );
    sun->rigidbody.setIsStatic(true);
    physics.addObject(sun);

    // ===== ЗЕМЛЯ =====
    Plane* ground = CreatePlane(render, vec3(0, -1, 0), 100.0f);
    ground->transform.scale = vec3(50, 1, 50);
    ground->rigidbody.setIsStatic(true);
    ground->setColliderSize(vec3(50.0f, 0.1f, 50.0f));
    physics.addObject(ground);

    // ===== КУБ (ОРИЕНТИР) =====
    Cube* cube = render.addObject<Cube>();
    cube->transform.position = (vec3(0, 0, 0));
    cube->transform.scale = (vec3(1, 1, 1));
    physics.addObject(cube);

    // 🚀 запуск
    render.run();

    return 0;
}