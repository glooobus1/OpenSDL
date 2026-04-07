#pragma once
#include "SceneObject.hpp"
#include <GL/glew.h>

class Sun : public SceneObject {
private:
    vec3 lightColor;
    float intensity;
    vec3 lightPosition;
    mat4 lightSpaceMatrix;
    float orthoSize;

public:
    Sun(const vec3& position = vec3(5, 10, 5),
        const vec3& color = vec3(1, 1, 1),
        float intens = 1.0f,
        float orthoSize = 15.0f)
        : lightPosition(position), lightColor(color), intensity(intens), orthoSize(orthoSize) {

        transform.position = position;
        updateLightSpaceMatrix();
    }

    void updateLightSpaceMatrix() {
        mat4 lightProjection = mat4::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 0.1f, 50.0f);
        mat4 lightView = mat4::lookAt(transform.position, vec3(0, 0, 0), vec3(0, 1, 0));
        lightSpaceMatrix = lightProjection * lightView;
    }

    void render() override {
        float pos[] = { transform.position.x, transform.position.y, transform.position.z, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, pos);

        float ambient[] = {
            lightColor.x * intensity * 0.3f,
            lightColor.y * intensity * 0.3f,
            lightColor.z * intensity * 0.3f,
            1.0f
        };
        float diffuse[] = {
            lightColor.x * intensity,
            lightColor.y * intensity,
            lightColor.z * intensity,
            1.0f
        };

        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    }

    void update(float deltaTime) override {}

    mat4 getLightSpaceMatrix() const { return lightSpaceMatrix; }
    void setLightSpaceMatrix(const mat4& m) { lightSpaceMatrix = m; }
    void setOrthoSize(float size) { orthoSize = size; updateLightSpaceMatrix(); }

    void setLightColor(const vec3& color) { lightColor = color; }
    vec3 getLightColor() const { return lightColor; }

    void setIntensity(float intens) { intensity = intens; }
    float getIntensity() const { return intensity; }

    void setPosition(const vec3& pos) {
        transform.position = pos;
        lightPosition = pos;
        updateLightSpaceMatrix();
    }

    vec3 getPosition() const { return transform.position; }
};