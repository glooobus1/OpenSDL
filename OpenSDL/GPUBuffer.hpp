#pragma once
#include <GL/glew.h>
#include <vector>

class GPUBuffer {
private:
    unsigned int m_ssbo;
    size_t m_size;
    unsigned int m_bindingPoint;

public:
    GPUBuffer() : m_ssbo(0), m_size(0), m_bindingPoint(0) {}

    ~GPUBuffer() {
        if (m_ssbo != 0) {
            glDeleteBuffers(1, &m_ssbo);
        }
    }

    void create(size_t size, unsigned int bindingPoint) {
        m_size = size;
        m_bindingPoint = bindingPoint;

        glGenBuffers(1, &m_ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void upload(const void* data, size_t size) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, data);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void bind() {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_bindingPoint, m_ssbo);
    }
};