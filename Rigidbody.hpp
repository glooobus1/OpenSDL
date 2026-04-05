#pragma once
#include "vec3.hpp"
#include "transform.hpp"

// Forward declaration
class SceneObject;

class Rigidbody {
private:
    vec3 m_velocity;
    vec3 m_acceleration;
    float m_mass;
    bool m_useGravity;
    bool m_isStatic;
    float m_restitution;
    float m_friction;

public:
    Rigidbody()
        : m_velocity(0, 0, 0), m_acceleration(0, 0, 0), m_mass(1.0f),
        m_useGravity(true), m_isStatic(false), m_restitution(0.5f), m_friction(0.1f) {
    }

    vec3 getVelocity() const { return m_velocity; }
    void setVelocity(const vec3& v) { m_velocity = v; }

    vec3 getAcceleration() const { return m_acceleration; }

    float getMass() const { return m_mass; }
    void setMass(float mass) { m_mass = mass; }

    bool useGravity() const { return m_useGravity; }
    void setUseGravity(bool use) { m_useGravity = use; }

    bool isStatic() const { return m_isStatic; }
    void setIsStatic(bool s) { m_isStatic = s; }

    float getRestitution() const { return m_restitution; }
    void setRestitution(float r) { m_restitution = r; }

    float getFriction() const { return m_friction; }
    void setFriction(float f) { m_friction = f; }

    void addForce(const vec3& force) {
        if (m_isStatic) return;
        m_acceleration = m_acceleration + (force / m_mass);
    }

    void addImpulse(const vec3& impulse) {
        if (m_isStatic) return;
        m_velocity = m_velocity + (impulse / m_mass);
    }

    void update(Transform& transform, float dt, const vec3& gravity) {
        if (m_isStatic) return;

        if (m_useGravity) {
            m_acceleration = m_acceleration + gravity;
        }

        m_velocity = m_velocity + m_acceleration * dt;
        m_velocity = m_velocity * (1.0f - m_friction * dt);
        transform.position = transform.position + m_velocity * dt;
        m_acceleration = vec3(0, 0, 0);
    }
};