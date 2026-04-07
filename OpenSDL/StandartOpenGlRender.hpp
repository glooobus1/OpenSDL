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

// Forward declaration
class CameraObject;
class Sun;

class StandartOpenGlRender {
private:
    TextRenderer m_textRenderer;
    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;
    int windowWidth = 800;
    int windowHeight = 600;
    bool running = true;

    PhysicsWorld m_physicsWorld;
    Camera camera;
    std::vector<std::unique_ptr<SceneObject>> sceneObjects;

    float lastTime = 0;
    float deltaTime = 0;
    int fps = 0;
    int frameCount = 0;

    ShadowMap shadowMap;
    Shader shadowShader;
    Shader mainShader;
    bool shadowsEnabled = true;

    std::function<void(float)> m_updateCallback;
    std::function<void(float)> m_fixedUpdateCallback;
    float m_fixedTimestep = 1.0f / 60.0f;
    float m_accumulator = 0.0f;

    SkyTexture m_skyTexture;

public:
    TextRenderer& getTextRenderer() { return m_textRenderer; }
    StandartOpenGlRender();
    ~StandartOpenGlRender();

    PhysicsWorld& getPhysicsWorld() { return m_physicsWorld; }

    bool init(int width = 800, int height = 600, const char* title = "3D Engine");
    void run();
    void stop();
    void setWindowTitle(const std::string& title);

    template<typename T, typename... Args>
    T* addObject(Args&&... args) {
        auto obj = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = obj.get();
        sceneObjects.push_back(std::move(obj));
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

    void setShadowsEnabled(bool enabled) { shadowsEnabled = enabled; }

    Camera& getCamera() { return camera; }
    SDL_Window* getWindow() { return window; }
    bool isRunning() const { return running; }
    int getFPS() const { return fps; }
    float getDeltaTime() const { return deltaTime; }
};