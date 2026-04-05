#include "PhysicsWorld.hpp"
#include "StandartOpenGlRender.hpp"
#include <algorithm>

PhysicsWorld::PhysicsWorld() : m_gravity(0, -9.81f, 0) {}

PhysicsWorld::~PhysicsWorld() {}

void PhysicsWorld::setGravity(const vec3& g) { m_gravity = g; }

vec3 PhysicsWorld::getGravity() const { return m_gravity; }

void PhysicsWorld::setCollisionCallback(std::function<void(const CollisionInfo&)> callback) {
    m_collisionCallback = callback;
}

void PhysicsWorld::addObject(SceneObject* obj) {
    if (obj && std::find(m_objects.begin(), m_objects.end(), obj) == m_objects.end()) {
        m_objects.push_back(obj);
    }
}

void PhysicsWorld::removeObject(SceneObject* obj) {
    auto it = std::find(m_objects.begin(), m_objects.end(), obj);
    if (it != m_objects.end()) {
        m_objects.erase(it);
    }
}

void PhysicsWorld::clear() {
    m_objects.clear();
}

// AABB vs AABB коллизия
static bool checkBoxBox(SceneObject* a, SceneObject* b, CollisionInfo& info) {
    vec3 minA, maxA, minB, maxB;
    a->collider.getWorldBounds(a->transform, minA, maxA);
    b->collider.getWorldBounds(b->transform, minB, maxB);

    if (minA.x > maxB.x || maxA.x < minB.x) return false;
    if (minA.y > maxB.y || maxA.y < minB.y) return false;
    if (minA.z > maxB.z || maxA.z < minB.z) return false;

    float dx = std::min(maxA.x - minB.x, maxB.x - minA.x);
    float dy = std::min(maxA.y - minB.y, maxB.y - minA.y);
    float dz = std::min(maxA.z - minB.z, maxB.z - minA.z);

    vec3 normal;
    float penetration;

    if (dx <= dy && dx <= dz) {
        normal = vec3(1, 0, 0);
        penetration = dx;
        if (a->transform.position.x > b->transform.position.x) normal = -normal;
    }
    else if (dy <= dx && dy <= dz) {
        normal = vec3(0, 1, 0);
        penetration = dy;
        if (a->transform.position.y > b->transform.position.y) normal = -normal;
    }
    else {
        normal = vec3(0, 0, 1);
        penetration = dz;
        if (a->transform.position.z > b->transform.position.z) normal = -normal;
    }

    info.objectA = a;
    info.objectB = b;
    info.normal = normal;
    info.point = (minA + maxA) * 0.5f;
    info.penetration = penetration;

    return true;
}

// НЕПРЕРЫВНАЯ КОЛЛИЗИЯ: проверка движения от старой позиции к новой
static bool checkSweptBoxBox(SceneObject* a, SceneObject* b,
    const vec3& oldPosA, const vec3& oldPosB,
    CollisionInfo& info, float dt) {
    vec3 minA, maxA, minB, maxB;

    // Получаем размеры коллайдеров
    vec3 halfSizeA = a->collider.getSize();
    vec3 halfSizeB = b->collider.getSize();

    // Текущие границы
    vec3 curMinA = a->transform.position - halfSizeA;
    vec3 curMaxA = a->transform.position + halfSizeA;
    vec3 curMinB = b->transform.position - halfSizeB;
    vec3 curMaxB = b->transform.position + halfSizeB;

    // Если уже пересекаются
    if (!(curMinA.x > curMaxB.x || curMaxA.x < curMinB.x ||
        curMinA.y > curMaxB.y || curMaxA.y < curMinB.y ||
        curMinA.z > curMaxB.z || curMaxA.z < curMinB.z)) {
        return checkBoxBox(a, b, info);
    }

    // Скорость объекта A относительно B
    vec3 velA = a->rigidbody.getVelocity();
    vec3 velB = b->rigidbody.getVelocity();
    vec3 relVel = velA - velB;

    // Если скорость нулевая, используем обычную проверку
    if (relVel.length() < 0.001f) return false;

    // Расширяем AABB объекта A в направлении движения
    vec3 expandedMin = oldPosA - halfSizeA;
    vec3 expandedMax = oldPosA + halfSizeA;

    // Добавляем скорость к расширенным границам
    if (relVel.x > 0) expandedMax.x += relVel.x * dt;
    else expandedMin.x += relVel.x * dt;

    if (relVel.y > 0) expandedMax.y += relVel.y * dt;
    else expandedMin.y += relVel.y * dt;

    if (relVel.z > 0) expandedMax.z += relVel.z * dt;
    else expandedMin.z += relVel.z * dt;

    // Проверяем пересечение расширенного AABB с B
    vec3 minB_static = b->transform.position - halfSizeB;
    vec3 maxB_static = b->transform.position + halfSizeB;

    if (expandedMin.x > maxB_static.x || expandedMax.x < minB_static.x) return false;
    if (expandedMin.y > maxB_static.y || expandedMax.y < minB_static.y) return false;
    if (expandedMin.z > maxB_static.z || expandedMax.z < minB_static.z) return false;

    // Вычисляем время столкновения (упрощённо)
    float t = 0.5f;

    info.objectA = a;
    info.objectB = b;
    info.normal = (b->transform.position - a->transform.position).normalize();
    info.penetration = 0.1f;

    return true;
}

static bool checkSphereSphere(SceneObject* a, SceneObject* b, CollisionInfo& info) {
    float radiusA = a->collider.getSize().x;
    float radiusB = b->collider.getSize().x;

    vec3 delta = b->transform.position - a->transform.position;
    float distance = delta.length();
    float minDist = radiusA + radiusB;

    if (distance < minDist) {
        info.objectA = a;
        info.objectB = b;
        info.normal = delta / distance;
        info.point = a->transform.position + info.normal * radiusA;
        info.penetration = minDist - distance;
        return true;
    }
    return false;
}

static bool checkCollision(SceneObject* a, SceneObject* b, CollisionInfo& info, float dt) {
    if (a->collider.getType() == ColliderType::Box &&
        b->collider.getType() == ColliderType::Box) {
        return checkBoxBox(a, b, info);
    }
    else if (a->collider.getType() == ColliderType::Sphere &&
        b->collider.getType() == ColliderType::Sphere) {
        return checkSphereSphere(a, b, info);
    }
    else {
        return checkSphereSphere(a, b, info);
    }
}

static void resolveCollision(const CollisionInfo& collision) {
    if (!collision.objectA || !collision.objectB) return;

    Rigidbody& rbA = collision.objectA->rigidbody;
    Rigidbody& rbB = collision.objectB->rigidbody;

    if (rbA.isStatic() && rbB.isStatic()) return;

    vec3 correction = collision.normal * collision.penetration;
    if (!rbA.isStatic()) {
        collision.objectA->transform.position = collision.objectA->transform.position - correction;
    }
    if (!rbB.isStatic()) {
        collision.objectB->transform.position = collision.objectB->transform.position + correction;
    }

    float e = (rbA.getRestitution() + rbB.getRestitution()) / 2.0f;
    vec3 relativeVelocity = rbB.getVelocity() - rbA.getVelocity();
    float velocityAlongNormal = relativeVelocity.dot(collision.normal);

    if (velocityAlongNormal > 0) return;

    float massA = rbA.isStatic() ? 999999.0f : rbA.getMass();
    float massB = rbB.isStatic() ? 999999.0f : rbB.getMass();
    float impulseMagnitude = -(1 + e) * velocityAlongNormal;
    impulseMagnitude /= (1.0f / massA + 1.0f / massB);

    vec3 impulse = collision.normal * impulseMagnitude;

    if (!rbA.isStatic()) {
        rbA.addImpulse(-impulse);
    }
    if (!rbB.isStatic()) {
        rbB.addImpulse(impulse);
    }
}

void PhysicsWorld::update(float dt) {
    // Сохраняем старые позиции
    std::vector<vec3> oldPositions;
    for (auto* obj : m_objects) {
        oldPositions.push_back(obj->transform.position);
    }

    // Обновление позиций
    for (auto* obj : m_objects) {
        if (obj) {
            obj->rigidbody.update(obj->transform, dt, m_gravity);
        }
    }

    // МНОГОКРАТНЫЕ ИТЕРАЦИИ ДЛЯ СТАБИЛЬНОСТИ
    int iterations = 4;  // 4 итерации для лучшей стабильности
    for (int iter = 0; iter < iterations; iter++) {
        for (size_t i = 0; i < m_objects.size(); i++) {
            for (size_t j = i + 1; j < m_objects.size(); j++) {
                if (m_objects[i] && m_objects[j]) {
                    CollisionInfo collision;
                    if (checkCollision(m_objects[i], m_objects[j], collision, dt)) {
                        resolveCollision(collision);
                        if (m_collisionCallback) {
                            m_collisionCallback(collision);
                        }
                    }
                }
            }
        }
    }
}