#pragma once
#include "vec3.hpp"
#include "transform.hpp"

enum class ColliderType {
    Sphere,
    Box,
    None
};

class Collider {
private:
    ColliderType m_type;
    vec3 m_center;
    vec3 m_size;

public:
    Collider() : m_type(ColliderType::None), m_center(0, 0, 0), m_size(1, 1, 1) {}

    void setSphere(float radius) {
        m_type = ColliderType::Sphere;
        m_size = vec3(radius, radius, radius);
    }

    void setBox(const vec3& halfSize) {
        m_type = ColliderType::Box;
        m_size = halfSize;
    }

    ColliderType getType() const { return m_type; }
    vec3 getSize() const { return m_size; }

    void getWorldBounds(const Transform& transform, vec3& min, vec3& max) const {
        if (m_type == ColliderType::Box) {
            min = transform.position - m_size;
            max = transform.position + m_size;
        }
        else if (m_type == ColliderType::Sphere) {
            float r = m_size.x;
            min = transform.position - vec3(r, r, r);
            max = transform.position + vec3(r, r, r);
        }
    }
};