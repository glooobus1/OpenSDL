#pragma once
#include "camera.hpp"
#include <SDL3/SDL.h>

class CameraController {
private:
    Camera* m_camera;
    float m_yaw;
    float m_pitch;
    float m_moveSpeed;
    float m_mouseSensitivity;
    bool m_mouseLocked;
    bool m_invertX;
    bool m_invertY;
    SDL_Window* m_window;
    int m_windowWidth;
    int m_windowHeight;

    // Физика ходьбы
    float m_verticalVelocity;
    bool m_isGrounded;
    float m_gravity;
    float m_jumpPower;

public:
    CameraController() : m_camera(nullptr), m_yaw(0), m_pitch(0),
        m_moveSpeed(5.0f), m_mouseSensitivity(0.005f),
        m_mouseLocked(false),
        m_invertX(true),
        m_invertY(true),
        m_window(nullptr),
        m_windowWidth(800), m_windowHeight(600),
        m_verticalVelocity(0.0f),
        m_isGrounded(true),
        m_gravity(20.0f),
        m_jumpPower(8.0f) {
    }

    void init(Camera* camera, SDL_Window* window, int width, int height) {
        m_camera = camera;
        m_window = window;
        m_windowWidth = width;
        m_windowHeight = height;

        vec3 dir = m_camera->forward().normalize();
        m_yaw = atan2(dir.x, dir.z);
        m_pitch = asin(-dir.y);
    }

    void handleMouseMotion(int dx, int dy) {
        if (!m_mouseLocked) return;

        if (m_invertX) {
            m_yaw -= dx * m_mouseSensitivity;
        }
        else {
            m_yaw += dx * m_mouseSensitivity;
        }

        if (m_invertY) {
            m_pitch -= dy * m_mouseSensitivity;
        }
        else {
            m_pitch += dy * m_mouseSensitivity;
        }

        if (m_pitch > 1.3f) m_pitch = 1.3f;
        if (m_pitch < -1.3f) m_pitch = -1.3f;

        quat qPitch = quat::fromAxisAngle(vec3(1, 0, 0), m_pitch);
        quat qYaw = quat::fromAxisAngle(vec3(0, 1, 0), m_yaw);
        m_camera->setRotation(qYaw * qPitch);
    }

    void update(float dt, const bool* keys) {
        if (!m_camera) return;

        vec3 moveDir(0, 0, 0);
        vec3 forward = m_camera->forward();
        vec3 right = m_camera->right();

        // Обнуляем Y для движения по горизонтали (не взлетаем)
        forward.y = 0;
        right.y = 0;
        forward = forward.normalize();
        right = right.normalize();

        // Горизонтальное движение
        if (keys[SDL_SCANCODE_W]) moveDir += forward;
        if (keys[SDL_SCANCODE_S]) moveDir -= forward;
        if (keys[SDL_SCANCODE_A]) moveDir -= right;
        if (keys[SDL_SCANCODE_D]) moveDir += right;

        // Нормализуем диагональное движение
        if (moveDir.length() > 0.0f) {
            moveDir = moveDir.normalize();
        }

        // Прыжок
        if (keys[SDL_SCANCODE_SPACE] && m_isGrounded) {
            m_verticalVelocity = m_jumpPower;
            m_isGrounded = false;
        }

        // Применяем гравитацию
        m_verticalVelocity -= m_gravity * dt;

        // Обновляем позицию по горизонтали
        vec3 newPos = m_camera->getTransform().position;
        newPos += moveDir * m_moveSpeed * dt;

        // Обновляем позицию по вертикали
        newPos.y += m_verticalVelocity * dt;

        // Простая проверка пола (если упали ниже 1.5, ставим на землю)
        if (newPos.y <= 1.5f) {
            newPos.y = 1.5f;
            m_verticalVelocity = 0.0f;
            m_isGrounded = true;
        }
        else {
            m_isGrounded = false;
        }

        m_camera->setPosition(newPos);
    }

    void setInvertX(bool invert) { m_invertX = invert; }
    void setInvertY(bool invert) { m_invertY = invert; }
    bool isInvertX() const { return m_invertX; }
    bool isInvertY() const { return m_invertY; }

    void setMoveSpeed(float speed) { m_moveSpeed = speed; }
    void setJumpPower(float power) { m_jumpPower = power; }
    void setGravity(float gravity) { m_gravity = gravity; }

    void setMouseLocked(bool locked) {
        m_mouseLocked = locked;
        if (m_window) {
            SDL_SetWindowRelativeMouseMode(m_window, locked);
        }
    }

    bool isMouseLocked() const { return m_mouseLocked; }

    void setPosition(const vec3& pos) {
        if (m_camera) m_camera->setPosition(pos);
    }

    vec3 getPosition() const {
        return m_camera ? m_camera->getTransform().position : vec3(0, 0, 0);
    }

    vec3 getForward() const {
        return m_camera ? m_camera->forward() : vec3(0, 0, 1);
    }

    vec3 getRight() const {
        return m_camera ? m_camera->right() : vec3(1, 0, 0);
    }

    vec3 getUp() const {
        return m_camera ? m_camera->up() : vec3(0, 1, 0);
    }

    void setYaw(float yaw) { m_yaw = yaw; }
    void setPitch(float pitch) { m_pitch = pitch; }
    float getYaw() const { return m_yaw; }
    float getPitch() const { return m_pitch; }

    bool isGrounded() const { return m_isGrounded; }
    float getVerticalVelocity() const { return m_verticalVelocity; }
};