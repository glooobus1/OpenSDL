#pragma once
#include <SDL3/SDL.h>
#include "camera.hpp"
#include "transform.hpp"
#include <vector>
#include <memory>

// Базовый класс для всех объектов сцены
class SceneObject {
public:
    Transform transform;
    virtual void render() = 0;
    virtual void update(float deltaTime) {}
    virtual ~SceneObject() {}
};

// Стандартный рендерер
class StandartOpenGlRender {
private:
    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;
    int windowWidth = 800;
    int windowHeight = 600;
    bool running = true;

    Camera camera;
    std::vector<std::unique_ptr<SceneObject>> sceneObjects;

    float lastTime = 0;
    float deltaTime = 0;
    int fps = 0;
    int frameCount = 0;

    // Параметры управления камерой
    float cameraMoveSpeed = 5.0f;
    float mouseSensitivity = 0.005f;
    bool rightMouseDown = false;

    // Углы поворота камеры (радианы)
    float cameraYaw = 0.0f;
    float cameraPitch = 0.0f;

public:
    StandartOpenGlRender();
    ~StandartOpenGlRender();

    bool init(int width = 800, int height = 600, const char* title = "3D Engine");
    void run();
    void stop();

    // Добавление объектов в сцену
    template<typename T, typename... Args>
    T* addObject(Args&&... args) {
        auto obj = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = obj.get();
        sceneObjects.push_back(std::move(obj));
        return ptr;
    }

    Camera& getCamera() { return camera; }
    bool isRunning() const { return running; }
    int getFPS() const { return fps; }
    float getDeltaTime() const { return deltaTime; }
};