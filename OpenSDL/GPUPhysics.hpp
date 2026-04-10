#pragma once
#include <GL/glew.h>
#include <vector>
#include <iostream>
#include "vec3.hpp"

// ============================================
// ПРАВИЛЬНАЯ СТРУКТУРА ДЛЯ GPU (alignment 16 байт)
// ============================================
struct alignas(16) GPURigidbody {
    vec3 position; float _pad1;      // 16 байт
    vec3 velocity; float _pad2;      // 16 байт
    vec3 acceleration; float _pad3;  // 16 байт

    // Свойства (упакованы в 16 байт)
    float mass;
    int isStatic;
    float restitution;
    float friction;

    float inverseMass;
    float padding[3];  // доводим до 16 байт
};

class GPUPhysics {
private:
    unsigned int m_ssbo;
    unsigned int m_computeProgram;
    int m_objectCount;
    size_t m_bufferSize;

public:
    GPUPhysics() : m_ssbo(0), m_computeProgram(0), m_objectCount(0), m_bufferSize(0) {}

    ~GPUPhysics() {
        if (m_ssbo) glDeleteBuffers(1, &m_ssbo);
        if (m_computeProgram) glDeleteProgram(m_computeProgram);
    }

    bool init(int maxObjects) {
        m_objectCount = maxObjects;
        m_bufferSize = maxObjects * sizeof(GPURigidbody);

        glGenBuffers(1, &m_ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_bufferSize, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // ИСПРАВЛЕННЫЙ COMPUTE SHADER
        const char* computeSource = R"(
#version 430 core

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

// СТРУКТУРА ДОЛЖНА ТОЧНО СОВПАДАТЬ С C++
struct Rigidbody {
    vec3 position; float pad1;
    vec3 velocity; float pad2;
    vec3 acceleration; float pad3;
    float mass;
    int isStatic;
    float restitution;
    float friction;
    float inverseMass;
    float padding[3];
};

layout(std430, binding = 2) buffer PhysicsBuffer {
    Rigidbody bodies[];
};

uniform float uDeltaTime;
uniform vec3 uGravity;
uniform int uObjectCount;

void main() {
    uint id = gl_GlobalInvocationID.x;
    if (id >= uObjectCount) return;
    
    // Работаем напрямую (без копии!)
    if (bodies[id].isStatic == 1) return;
    
    // Гравитация
    bodies[id].acceleration += uGravity;
    
    // Обновление скорости
    bodies[id].velocity += bodies[id].acceleration * uDeltaTime;
    
    // Безопасное трение
    float damp = clamp(1.0 - bodies[id].friction * uDeltaTime, 0.0, 1.0);
    bodies[id].velocity *= damp;
    
    // Обновление позиции
    bodies[id].position += bodies[id].velocity * uDeltaTime;
    
    // Простая граница пола
    if (bodies[id].position.y < -1.5 && bodies[id].velocity.y < 0) {
        bodies[id].position.y = -1.5;
        bodies[id].velocity.y *= -bodies[id].restitution;
    }
    
    // Сброс ускорения
    bodies[id].acceleration = vec3(0.0);
}
)";

        m_computeProgram = createComputeShader(computeSource);
        if (!m_computeProgram) return false;

        std::cout << "GPU Physics initialized for " << maxObjects << " objects" << std::endl;
        return true;
    }

    void uploadData(const std::vector<GPURigidbody>& bodies) {
        if (bodies.size() > (size_t)m_objectCount) {
            std::cout << "Warning: Too many bodies for GPU buffer!" << std::endl;
            return;
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bodies.size() * sizeof(GPURigidbody), bodies.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void update(float dt, const vec3& gravity, int count) {
        if (count <= 0) return;

        // ВАЖНО: bind перед каждым dispatch
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_ssbo);

        glUseProgram(m_computeProgram);
        glUniform1f(glGetUniformLocation(m_computeProgram, "uDeltaTime"), dt);
        glUniform3f(glGetUniformLocation(m_computeProgram, "uGravity"), gravity.x, gravity.y, gravity.z);
        glUniform1i(glGetUniformLocation(m_computeProgram, "uObjectCount"), count);

        int groups = (count + 255) / 256;
        glDispatchCompute(groups, 1, 1);

        // Барьер перед чтением
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
    }

    void downloadResults(std::vector<GPURigidbody>& outBodies) {
        if (outBodies.empty()) return;

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, outBodies.size() * sizeof(GPURigidbody), outBodies.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    bool isValid() const { return m_computeProgram != 0; }

    void bind() {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_ssbo);
    }

    unsigned int getBufferId() const { return m_ssbo; }

private:
    unsigned int createComputeShader(const char* source) {
        unsigned int shader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512];
            glGetShaderInfoLog(shader, 512, nullptr, log);
            std::cout << "Compute shader compile error:\n" << log << std::endl;
            return 0;
        }

        unsigned int program = glCreateProgram();
        glAttachShader(program, shader);
        glLinkProgram(program);

        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char log[512];
            glGetProgramInfoLog(program, 512, nullptr, log);
            std::cout << "Compute program link error:\n" << log << std::endl;
            return 0;
        }

        glDeleteShader(shader);
        return program;
    }
};