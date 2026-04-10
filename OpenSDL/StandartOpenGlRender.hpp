#pragma once
#include <SDL3/SDL.h>
#include "camera.hpp"
#include "transform.hpp"
#include "ShadowMap.hpp"
#include "Shader.hpp"
#include "ObjMesh.hpp"
#include "Collider.hpp"
#include <functional> 
#include <vector>
#include <memory>
#include "TextRenderer.hpp" 
#include "PhysicsWorld.hpp"
#include "Rigidbody.hpp"
#include "SceneObject.hpp"
#include "SkyTexture.hpp"
#include "GPUBuffer.hpp"
#include "GPUPhysics.hpp"

// Forward declaration
class CameraObject;
class Sun;
class StandartPlayerController;

class StandartOpenGlRender {
private:
    TextRenderer m_textRenderer;
    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;
    int windowWidth = 800;
    int windowHeight = 600;
    bool running = true;

    PhysicsWorld m_physicsWorld;      // CPU физика (fallback)
    GPUPhysics m_gpuPhysics;          // GPU физика
    Camera camera;
    Camera* m_activeCamera = nullptr;
    std::vector<std::unique_ptr<SceneObject>> sceneObjects;

    float lastTime = 0;
    float deltaTime = 0;
    int fps = 0;
    int frameCount = 0;

    ShadowMap shadowMap;
    Shader shadowShader;
    Shader mainShader;

    // GPU-буферы для матриц
    GPUBuffer m_modelBuffer;
    GPUBuffer m_normalMatrixBuffer;
    std::vector<mat4> m_gpuModels;
    std::vector<mat4> m_gpuNormalMatrices;
    bool m_matricesDirty = true;

    std::function<void(float)> m_updateCallback;
    std::function<void(float)> m_fixedUpdateCallback;
    float m_fixedTimestep = 1.0f / 60.0f;
    float m_accumulator = 0.0f;

    SkyTexture m_skyTexture;
    unsigned int m_tessProgram;
    unsigned int m_tessVAO, m_tessVBO;
public:
    // ======================== НАСТРОЙКИ ========================
    // Изменяй эти переменные перед запуском

    // === ГРАФИКА ===
    bool ENABLE_SHADOWS = true;           // Тени (на слабых ПК выключить)
    bool ENABLE_VSYNC = false;            // Вертикальная синхронизация
    bool ENABLE_DEBUG_COLLIDERS = false;  // Показывать зелёные рамки коллайдеров
    bool ENABLE_GPU_MATRICES = true;      // GPU-буферы для матриц (OpenGL 4.3+)
    bool ENABLE_TESSELLATION = false;      // тескляция теней
    int MAX_GPU_OBJECTS = 5000;           // Максимум объектов для GPU-матриц

    // === ФИЗИКА ===
    bool ENABLE_GPU_PHYSICS = true;       // Физика на GPU (быстрее)
    int MAX_PHYSICS_OBJECTS = 5000;       // Максимум объектов для GPU-физики
    vec3 GRAVITY = vec3(0, -9.8f, 0);     // Гравитация (X, Y, Z)

    // === УПРАВЛЕНИЕ ===
    float PLAYER_MOVE_SPEED = 8.0f;       // Скорость движения игрока
    float PLAYER_MOUSE_SENS = 0.005f;     // Чувствительность мыши
    float PLAYER_JUMP_POWER = 5.5f;       // Сила прыжка
    int TESSELLATION_LEVEL = 4;      // сила теселяции теней
    float PLAYER_GRAVITY = 12.0f;         // Гравитация для игрока
    bool INVERT_X = true;                 // Инвертировать мышь по X
    bool INVERT_Y = true;                 // Инвертировать мышь по Y

    // === КАМЕРА ===
    float CAMERA_FOV = 80.0f;             // Поле зрения (градусы)
    float CAMERA_NEAR = 0.1f;             // Ближняя плоскость отсечения
    float CAMERA_FAR = 1000.0f;           // Дальняя плоскость отсечения
    vec3 CAMERA_START_POS = vec3(0, 3, 15); // Начальная позиция камеры

    // === МИР ===
    vec3 WORLD_CLEAR_COLOR = vec3(0.5f, 0.7f, 1.0f); // Цвет неба (RGB)
    // ==========================================================

    TextRenderer& getTextRenderer() { return m_textRenderer; }
    StandartOpenGlRender();
    ~StandartOpenGlRender();

    PhysicsWorld& getPhysicsWorld() { return m_physicsWorld; }
    GPUPhysics& getGPUPhysics() { return m_gpuPhysics; }

    bool init(int width = 800, int height = 600, const char* title = "3D Engine");
    void run();
    void stop();
    void setWindowTitle(const std::string& title);

    template<typename T, typename... Args>
    T* addObject(Args&&... args) {
        auto obj = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = obj.get();
        sceneObjects.push_back(std::move(obj));
        m_matricesDirty = true;
        return ptr;
    }

    void setUpdateCallback(std::function<void(float deltaTime)> callback) {
        m_updateCallback = callback;
    }

    void setFixedUpdateCallback(std::function<void(float deltaTime)> callback) {
        m_fixedUpdateCallback = callback;
    }

    void setFixedTimestep(float timestep) {
        m_fixedTimestep = timestep;
    }

    void setCamera(Camera* cam) { m_activeCamera = cam; }
    Camera& getCamera() {
        if (m_activeCamera) return *m_activeCamera;
        return camera;
    }

    SDL_Window* getWindow() { return window; }
    bool isRunning() const { return running; }
    int getFPS() const { return fps; }
    float getDeltaTime() const { return deltaTime; }

    void markMatricesDirty() { m_matricesDirty = true; }
    void uploadMatricesToGPU();

    void printSettings();  // Вывод всех настроек в консоль
};