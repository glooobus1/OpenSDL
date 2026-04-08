#pragma once
#include "vec2.hpp"
#include "vec3.hpp"
#include "quat.hpp"
#include "transform.hpp"
#include <SDL3/SDL.h>

class Camera {
private:
    Transform transform;
    float fov;           // поле зрения в градусах
    float fovStandard;   // стандартное FOV
    float fovOffset;     // смещение для прицеливания
    float nearPlane;     // ближняя плоскость отсечения
    float farPlane;      // дальняя плоскость отсечения

public:
    // Конструктор
    Camera() : fov(80.0f), fovStandard(80.0f), fovOffset(0.0f), nearPlane(0.1f), farPlane(1000.0f) {}

    Camera(float fov, float nearPlane, float farPlane)
        : fov(fov), fovStandard(fov), fovOffset(0.0f), nearPlane(nearPlane), farPlane(farPlane) {
    }

    // Управление трансформацией камеры
    void setPosition(const vec3& pos) { transform.position = pos; }
    void setRotation(const quat& rot) { transform.rotation = rot; }
    void move(const vec3& delta) { transform.position = transform.position + delta; }
    void rotate(const quat& delta) { transform.rotation = delta * transform.rotation; }

    // Получение матриц
    mat4 getViewMatrix() const {
        mat4 rot = transform.rotation.toMatrix().transpose();
        mat4 trans = mat4::translation(-transform.position);
        return rot * trans;
    }

    mat4 getProjectionMatrix(float aspect) const {
        return mat4::perspective(
            (fovStandard + fovOffset) * 3.14159f / 180.0f,
            aspect,
            nearPlane,
            farPlane
        );
    }

    // Направления взгляда
    vec3 forward() const { return transform.rotation * vec3(0, 0, 1); }
    vec3 right() const { return transform.rotation * vec3(1, 0, 0); }
    vec3 up() const { return transform.rotation * vec3(0, 1, 0); }

    // Смотреть на точку
    void lookAt(const vec3& target) {
        transform.lookAt(target, vec3(0, 1, 0));
    }

    // Управление FOV
    float getFov() const { return fovStandard + fovOffset; }
    void setFov(float f) {
        fovStandard = f;
        if (fovStandard < 10.0f) fovStandard = 10.0f;
        if (fovStandard > 120.0f) fovStandard = 120.0f;
    }
    
    void setAiming(bool aiming) {
        if (aiming) {
            fovOffset = -40.0f;
        }
        else {
            fovOffset = 0.0f;
        }
    }
    vec3 getPosition() const { return transform.position; }
    void addFovOffset(float delta) {
        fovOffset += delta;
        if (fovOffset > 0.0f) fovOffset = 0.0f;
        if (fovOffset < -fovStandard) fovOffset = -fovStandard;
    }

    float getFovOffset() const { return fovOffset; }
    float getFovStandard() const { return fovStandard; }

    Transform& getTransform() { return transform; }
    const Transform& getTransform() const { return transform; }
};