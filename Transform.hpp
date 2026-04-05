#pragma once
#include "vec3.hpp"
#include "quat.hpp"
#include "mat4.hpp"

struct Transform {
    vec3 position;
    quat rotation;
    vec3 scale;

    // Конструкторы
    Transform() : position(0, 0, 0), rotation(), scale(1, 1, 1) {}

    Transform(const vec3& pos, const quat& rot, const vec3& scl = vec3(1, 1, 1))
        : position(pos), rotation(rot), scale(scl) {
    }

    // Получить матрицу трансформации (для шейдера)
    mat4 getMatrix() const {
        return mat4::translation(position) *
            rotation.toMatrix() *
            mat4::scale(scale);
    }

    // Направления (с учётом поворота)
    vec3 forward() const { return rotation * vec3(0, 0, 1); }
    vec3 right()   const { return rotation * vec3(1, 0, 0); }
    vec3 up()      const { return rotation * vec3(0, 1, 0); }

    // Перемещение в локальных координатах
    void moveLocal(const vec3& delta) {
        position = position + rotation * delta;
    }

    // Поворот относительно текущего поворота
    void rotate(const quat& delta) {
        rotation = delta * rotation; // порядок важен!
    }

    // Поворот вокруг мировой оси
    void rotateWorld(const vec3& axis, float angle) {
        rotation = quat::fromAxisAngle(axis, angle) * rotation;
    }

    // Поворот вокруг локальной оси
    void rotateLocal(const vec3& axis, float angle) {
        rotation = rotation * quat::fromAxisAngle(axis, angle);
    }

    // Смотреть на точку
    void lookAt(const vec3& target, const vec3& up = vec3(0, 1, 0)) {
        vec3 direction = (target - position).normalize();

        // Вычисляем кватернион из направления
        vec3 z = direction;
        vec3 x = up.cross(z).normalize();
        vec3 y = z.cross(x);

        // Из матрицы поворота в кватернион
        float trace = x.x + y.y + z.z;
        if (trace > 0) {
            float s = 0.5f / sqrt(trace + 1.0f);
            rotation.w = 0.25f / s;
            rotation.x = (y.z - z.y) * s;
            rotation.y = (z.x - x.z) * s;
            rotation.z = (x.y - y.x) * s;
        }
        else {
            // Обработка вырожденного случая
            if (x.x > y.y && x.x > z.z) {
                float s = 2.0f * sqrt(1.0f + x.x - y.y - z.z);
                rotation.w = (y.z - z.y) / s;
                rotation.x = 0.25f * s;
                rotation.y = (x.y + y.x) / s;
                rotation.z = (x.z + z.x) / s;
            }
            else if (y.y > z.z) {
                float s = 2.0f * sqrt(1.0f + y.y - x.x - z.z);
                rotation.w = (z.x - x.z) / s;
                rotation.x = (x.y + y.x) / s;
                rotation.y = 0.25f * s;
                rotation.z = (y.z + z.y) / s;
            }
            else {
                float s = 2.0f * sqrt(1.0f + z.z - x.x - y.y);
                rotation.w = (x.y - y.x) / s;
                rotation.x = (x.z + z.x) / s;
                rotation.y = (y.z + z.y) / s;
                rotation.z = 0.25f * s;
            }
        }
    }

    // Иерархия (для родителей/детей)
    Transform getRelativeTo(const Transform& parent) const {
        Transform result;
        result.position = parent.rotation.inverse() * (position - parent.position);
        result.rotation = parent.rotation.inverse() * rotation;
        result.scale = scale / parent.scale;
        return result;
    }

    // Комбинирование с родителем
    Transform combineWith(const Transform& parent) const {
        Transform result;
        result.position = parent.position + parent.rotation * (position * parent.scale);
        result.rotation = parent.rotation * rotation;
        result.scale = parent.scale * scale;
        return result;
    }

    // Интерполяция между двумя трансформациями
    Transform lerp(const Transform& other, float t) const {
        Transform result;
        result.position = position.lerp(other.position, t);
        result.rotation = rotation.slerp(other.rotation, t);
        result.scale.x = scale.x + (other.scale.x - scale.x) * t;
        result.scale.y = scale.y + (other.scale.y - scale.y) * t;
        result.scale.z = scale.z + (other.scale.z - scale.z) * t;
        return result;
    }

    // Печать
    void print() const {
        printf("Transform:\n");
        printf("  pos: (%.2f, %.2f, %.2f)\n", position.x, position.y, position.z);
        printf("  rot: (%.2f, %.2f, %.2f, %.2f)\n", rotation.x, rotation.y, rotation.z, rotation.w);
        printf("  scl: (%.2f, %.2f, %.2f)\n", scale.x, scale.y, scale.z);
    }
};