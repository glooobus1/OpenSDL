#pragma once
#include "vec2.hpp"
#include "vec3.hpp"
#include "quat.hpp"
#include "transform.hpp"
#include "mat4.hpp"

class Camera {
private:
    Transform transform;
    float fov;           // поле зрения в градусах
    float nearPlane;     // ближняя плоскость отсечения
    float farPlane;      // дальняя плоскость отсечения

public:
    // Конструкторы
    Camera() : fov(80.0f), nearPlane(0.1f), farPlane(1000.0f) {}

    Camera(float fov, float nearPlane, float farPlane)
        : fov(fov), nearPlane(nearPlane), farPlane(farPlane) {
    }

    // Прямое управление трансформацией (для рендера)
    void setPosition(const vec3& pos) { transform.position = pos; }
    void setRotation(const quat& rot) { transform.rotation = rot; }
    void setTransform(const Transform& t) { transform = t; }

    // Перемещение (относительное)
    void move(const vec3& delta) { transform.position = transform.position + delta; }
    void rotate(const quat& delta) { transform.rotation = delta * transform.rotation; }

    // Получение матриц для рендера
    mat4 getViewMatrix() const {
        mat4 rot = transform.rotation.toMatrix().transpose();
        mat4 trans = mat4::translation(-transform.position);
        return rot * trans;
    }

    mat4 getProjectionMatrix(float aspect) const {
        return mat4::perspective(
            fov * 3.14159f / 180.0f,
            aspect,
            nearPlane,
            farPlane
        );
    }

    // Направления взгляда (для рендера и внешнего управления)
    vec3 forward() const { return transform.rotation * vec3(0, 0, 1); }
    vec3 right() const { return transform.rotation * vec3(1, 0, 0); }
    vec3 up() const { return transform.rotation * vec3(0, 1, 0); }

    // Смотреть на точку
    void lookAt(const vec3& target) {
        transform.lookAt(target, vec3(0, 1, 0));
    }

    // Геттеры
    float getFov() const { return fov; }
    void setFov(float f) {
        fov = f;
        if (fov < 10.0f) fov = 10.0f;
        if (fov > 120.0f) fov = 120.0f;
    }

    vec3 getPosition() const { return transform.position; }
    quat getRotation() const { return transform.rotation; }
    Transform& getTransform() { return transform; }
    const Transform& getTransform() const { return transform; }
};