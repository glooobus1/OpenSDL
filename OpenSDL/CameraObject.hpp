#pragma once
#include "StandartOpenGlRender.hpp"

class CameraObject : public SceneObject {
private:
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

    SDL_Window* m_window;

public:
    CameraObject() : m_yaw(0), m_pitch(0),
        m_moveSpeed(5.0f), m_mouseSensitivity(0.005f),
        m_mouseLocked(false),
        m_invertX(true), m_invertY(true),
        m_verticalVelocity(0.0f), m_isGrounded(true),
        m_gravity(20.0f), m_jumpPower(8.0f),
        m_window(nullptr) {
    }

    void init(SDL_Window* window) {
        m_window = window;
        vec3 dir = transform.forward().normalize();
        m_yaw = atan2(dir.x, dir.z);
        m_pitch = asin(-dir.y);
    }

    void handleMouseMotion(int dx, int dy) {
        if (!m_mouseLocked) return;

        if (m_invertX) m_yaw -= dx * m_mouseSensitivity;
        else m_yaw += dx * m_mouseSensitivity;

        if (m_invertY) m_pitch -= dy * m_mouseSensitivity;
        else m_pitch += dy * m_mouseSensitivity;

        if (m_pitch > 1.3f) m_pitch = 1.3f;
        if (m_pitch < -1.3f) m_pitch = -1.3f;

        quat qPitch = quat::fromAxisAngle(vec3(1, 0, 0), m_pitch);
        quat qYaw = quat::fromAxisAngle(vec3(0, 1, 0), m_yaw);
        transform.rotation = qYaw * qPitch;
    }

    void update(float dt) override {
        const bool* keys = SDL_GetKeyboardState(nullptr);

        vec3 moveDir(0, 0, 0);
        vec3 forward = transform.forward();
        vec3 right = transform.right();

        forward.y = 0;
        right.y = 0;
        forward = forward.normalize();
        right = right.normalize();

        if (keys[SDL_SCANCODE_W]) moveDir -= forward;
        if (keys[SDL_SCANCODE_S]) moveDir += forward;
        if (keys[SDL_SCANCODE_A]) moveDir -= right;
        if (keys[SDL_SCANCODE_D]) moveDir += right;

        if (moveDir.length() > 0.0f) {
            moveDir = moveDir.normalize();
        }

        if (keys[SDL_SCANCODE_SPACE] && m_isGrounded) {
            m_verticalVelocity = m_jumpPower;
            m_isGrounded = false;
        }

        m_verticalVelocity -= m_gravity * dt;

        vec3 newPos = transform.position;
        newPos += moveDir * m_moveSpeed * dt;
        newPos.y += m_verticalVelocity * dt;

        if (newPos.y <= 1.5f) {
            newPos.y = 1.5f;
            m_verticalVelocity = 0.0f;
            m_isGrounded = true;
        }
        else {
            m_isGrounded = false;
        }

        transform.position = newPos;

        // ESC для отпускания мыши
        if (keys[SDL_SCANCODE_ESCAPE]) {
            static bool wasPressed = false;
            if (!wasPressed) {
                m_mouseLocked = false;
                if (m_window) {
                    SDL_SetWindowRelativeMouseMode(m_window, false);
                }
            }
            wasPressed = true;
        }
        else {
            static bool wasPressed = false;
            wasPressed = false;
        }
    }

    void setMouseLocked(bool locked) {
        m_mouseLocked = locked;
        if (m_window) {
            SDL_SetWindowRelativeMouseMode(m_window, locked);
        }
    }

    bool isMouseLocked() const { return m_mouseLocked; }

    void render() override {}

    void setInvertX(bool invert) { m_invertX = invert; }
    void setInvertY(bool invert) { m_invertY = invert; }

    void setMoveSpeed(float speed) { m_moveSpeed = speed; }
    void setJumpPower(float power) { m_jumpPower = power; }
    void setGravity(float gravity) { m_gravity = gravity; }

    vec3 getEyePosition() const { return transform.position; }
    vec3 getForward() const { return transform.forward(); }
    vec3 getRight() const { return transform.right(); }

    void setPosition(const vec3& pos) { transform.position = pos; }
};