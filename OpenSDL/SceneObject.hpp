#pragma once
#include "transform.hpp"
#include "Rigidbody.hpp"
#include "Collider.hpp"

class SceneObject {
public:
    Transform transform;
    Rigidbody rigidbody;
    Collider collider;

    virtual void render() = 0;
    virtual void update(float deltaTime) {}
    virtual ~SceneObject() {}

    void setColliderSize(const vec3& halfSize) {
        collider.setBox(halfSize);
    }

    void setColliderRadius(float radius) {
        collider.setSphere(radius);
    }
};