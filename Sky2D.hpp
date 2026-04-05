#pragma once
#include "StandartOpenGlRender.hpp"
#include "Shader.hpp"
#include <iostream>

class Sky2D : public SceneObject {
private:
    Shader shader;
    unsigned int VAO, VBO, EBO;
    float time;

public:
    Sky2D() : time(0.0f) {
        setupQuad();

        if (!shader.loadFromString(
            "#version 330 core\n"
            "layout(location = 0) in vec2 aPos;\n"
            "layout(location = 1) in vec2 aTexCoord;\n"
            "out vec2 vUV;\n"
            "void main() {\n"
            "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
            "    vUV = aTexCoord;\n"
            "}\n",
            "#version 330 core\n"
            "in vec2 vUV;\n"
            "out vec4 FragColor;\n"
            "uniform float uTime;\n"
            "void main() {\n"
            "    vec2 uv = vUV;\n"
            "    vec3 skyColorTop = vec3(0.1, 0.3, 0.7);\n"
            "    vec3 skyColorBottom = vec3(0.5, 0.7, 1.0);\n"
            "    vec3 skyColor = mix(skyColorBottom, skyColorTop, uv.y);\n"
            "    vec2 sunPos = vec2(0.7, 0.85);\n"
            "    float sunRadius = 0.1;\n"
            "    float sunGlow = 0.18;\n"
            "    float distToSun = distance(uv, sunPos);\n"
            "    float sunIntensity = 1.0 - smoothstep(sunRadius - 0.02, sunRadius, distToSun);\n"
            "    vec3 sunColor = vec3(1.0, 0.85, 0.4);\n"
            "    float glowIntensity = 1.0 - smoothstep(sunRadius, sunGlow, distToSun);\n"
            "    vec3 glowColor = vec3(1.0, 0.6, 0.2);\n"
            "    vec2 cloudUV = uv * 2.5 + vec2(uTime * 0.05, 0.0);\n"
            "    float clouds = sin(cloudUV.x * 5.0) * sin(cloudUV.y * 3.0) * 0.3 + 0.3;\n"
            "    cloudMask = smoothstep(0.2, 0.6, uv.y);\n"
            "    clouds *= cloudMask;\n"
            "    clouds *= 1.0 - smoothstep(sunRadius - 0.05, sunRadius + 0.1, distToSun);\n"
            "    vec3 finalColor = skyColor;\n"
            "    finalColor += glowColor * glowIntensity * 0.5;\n"
            "    finalColor += sunColor * sunIntensity;\n"
            "    finalColor = mix(finalColor, vec3(1.0), clouds * 0.5);\n"
            "    FragColor = vec4(finalColor, 1.0);\n"
            "}\n"
        )) {
            std::cout << "Warning: Sky2D shader failed to load!" << std::endl;
        }
    }

    void setupQuad() {
        float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 1.0f
        };

        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void update(float dt) override {
        time += dt;
    }

    void render() override {
        // Сохраняем текущее состояние
        GLint oldDepthFunc;
        glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFunc);

        // Рисуем небо ПЕРЕД всем
        glDepthFunc(GL_ALWAYS);
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        shader.use();
        shader.setUniform("uTime", time);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Восстанавливаем
        glDepthFunc(oldDepthFunc);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
    }
};