#pragma once
#include "camera.hpp"
#include "Collider.hpp"
#include "PhysicsWorld.hpp"
#include "SceneObject.hpp"
#include <SDL3/SDL.h>

class StandartPlayerController {
private:
    Camera* m_camera;
    PhysicsWorld* m_physicsWorld;
    SDL_Window* m_window;

    // Управление
    float m_yaw;
    float m_pitch;
    float m_moveSpeed;
    float m_mouseSensitivity;
    bool m_mouseLocked;
    bool m_invertX;
    bool m_invertY;

    // Физика ходьбы
    float m_verticalVelocity;
    bool m_isGrounded;
    float m_gravity;
    float m_jumpPower;

    // Коллайдер
    Collider m_playerCollider;

public:
    StandartPlayerController()
        : m_camera(nullptr)
        , m_physicsWorld(nullptr)
        , m_window(nullptr)
        , m_yaw(0)
        , m_pitch(0)
        , m_moveSpeed(8.0f)
        , m_mouseSensitivity(0.005f)
        , m_mouseLocked(false)
        , m_invertX(true)
        , m_invertY(true)
        , m_verticalVelocity(0.0f)
        , m_isGrounded(true)
        , m_gravity(12.0f)
        , m_jumpPower(5.5f)
    {
        m_playerCollider.setBox(vec3(0.4f, 0.8f, 0.4f));
    }

    void init(Camera* camera, PhysicsWorld* physicsWorld, SDL_Window* window, int width, int height) {
        m_camera = camera;
        m_physicsWorld = physicsWorld;
        m_window = window;

        vec3 dir = m_camera->forward().normalize();
        m_yaw = atan2(dir.x, dir.z);
        m_pitch = asin(-dir.y);
    }

    void handleMouseMotion(int dx, int dy) {
        if (!m_mouseLocked || !m_camera) return;

        if (m_invertX) m_yaw -= dx * m_mouseSensitivity;
        else m_yaw += dx * m_mouseSensitivity;

        if (m_invertY) m_pitch -= dy * m_mouseSensitivity;
        else m_pitch += dy * m_mouseSensitivity;

        const float maxPitch = 1.57f;
        if (m_pitch > maxPitch) m_pitch = maxPitch;
        if (m_pitch < -maxPitch) m_pitch = -maxPitch;

        quat qPitch = quat::fromAxisAngle(vec3(1, 0, 0), m_pitch);
        quat qYaw = quat::fromAxisAngle(vec3(0, 1, 0), m_yaw);
        m_camera->setRotation(qYaw * qPitch);
    }

    void update(float dt, const bool* keys) {
        if (!m_camera) return;

        vec3 moveDir(0, 0, 0);
        vec3 forward = m_camera->forward();
        vec3 right = m_camera->right();

        forward.y = 0;
        right.y = 0;
        if (forward.length() > 0.01f) forward = forward.normalize();
        if (right.length() > 0.01f) right = right.normalize();

        // ИНВЕРТИРОВАННЫЕ W и S (W = назад, S = вперёд)
        if (keys[SDL_SCANCODE_W]) moveDir -= forward;
        if (keys[SDL_SCANCODE_S]) moveDir += forward;
        if (keys[SDL_SCANCODE_A]) moveDir -= right;
        if (keys[SDL_SCANCODE_D]) moveDir += right;

        if (moveDir.length() > 0.01f) moveDir = moveDir.normalize();

        // Прыжок
        static bool spaceWasPressed = false;
        bool spaceIsPressed = keys[SDL_SCANCODE_SPACE];
        if (spaceIsPressed && !spaceWasPressed && m_isGrounded) {
            m_verticalVelocity = m_jumpPower;
            m_isGrounded = false;
        }
        spaceWasPressed = spaceIsPressed;

        m_verticalVelocity -= m_gravity * dt;

        vec3 newPos = m_camera->getPosition();
        newPos += moveDir * m_moveSpeed * dt;
        newPos.y += m_verticalVelocity * dt;

        if (newPos.y <= 1.0f) {
            newPos.y = 1.0f;
            m_verticalVelocity = 0.0f;
            m_isGrounded = true;
        }
        else {
            m_isGrounded = false;
        }

        m_camera->setPosition(newPos);
    }

    // Сеттеры
    void setMoveSpeed(float speed) { m_moveSpeed = speed; }
    void setJumpPower(float power) { m_jumpPower = power; }
    void setGravity(float gravity) { m_gravity = gravity; }
    void setMouseSensitivity(float sens) { m_mouseSensitivity = sens; }
    void setInvertX(bool invert) { m_invertX = invert; }
    void setInvertY(bool invert) { m_invertY = invert; }

    void setMouseLocked(bool locked) {
        m_mouseLocked = locked;
        if (m_window) SDL_SetWindowRelativeMouseMode(m_window, locked);
    }

    // Геттеры
    bool isMouseLocked() const { return m_mouseLocked; }
    bool isGrounded() const { return m_isGrounded; }
    vec3 getPosition() const { return m_camera ? m_camera->getPosition() : vec3(0, 0, 0); }
};