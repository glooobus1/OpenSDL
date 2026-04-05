#pragma once
#include "vec3.hpp"
#include <vector>
#include <functional>

// Forward declaration
class SceneObject;

struct CollisionInfo {
    SceneObject* objectA;
    SceneObject* objectB;
    vec3 normal;
    vec3 point;
    float penetration;
};

class PhysicsWorld {
private:
    vec3 m_gravity;
    std::vector<SceneObject*> m_objects;
    std::function<void(const CollisionInfo&)> m_collisionCallback;

public:
    PhysicsWorld();
    ~PhysicsWorld();

    void setGravity(const vec3& g);
    vec3 getGravity() const;

    void setCollisionCallback(std::function<void(const CollisionInfo&)> callback);

    void addObject(SceneObject* obj);
    void removeObject(SceneObject* obj);
    void clear();
    void update(float dt);
};