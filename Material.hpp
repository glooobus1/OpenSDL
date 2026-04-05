#pragma once
#include "color.hpp"
#include "shader.hpp"

class Material {
private:
    Shader* shader;

    // Свойства материала
    Color ambient;
    Color diffuse;
    Color specular;
    float shininess;

    // Флаги и текстура (ИСПРАВЛЕНО: убраны лишние точки с запятой)
    bool hasTextureFlag;
    unsigned int textureId;

public:
    // Конструктор
    Material()
        : shader(nullptr)
        , ambient(Color::white())
        , diffuse(Color::white())
        , specular(Color::white())
        , shininess(32.0f)
        , hasTextureFlag(false)
        , textureId(0)
    {
    }

    // Деструктор
    ~Material() {}

    // Настройка шейдера
    void setShader(Shader* s) { shader = s; }
    Shader* getShader() const { return shader; }

    // Свойства
    void setAmbient(const Color& c) { ambient = c; }
    void setDiffuse(const Color& c) { diffuse = c; }
    void setSpecular(const Color& c) { specular = c; }
    void setShininess(float s) { shininess = s; }

    Color getAmbient() const { return ambient; }
    Color getDiffuse() const { return diffuse; }
    Color getSpecular() const { return specular; }
    float getShininess() const { return shininess; }

    // Текстура (ИСПРАВЛЕНО: правильные имена методов)
    void setTexture(unsigned int texId) {
        textureId = texId;
        hasTextureFlag = true;
    }

    bool hasTexture() const { return hasTextureFlag; }
    unsigned int getTextureId() const { return textureId; }  // ИСПРАВЛЕНО: теперь метод

    // Применение материала (ИСПРАВЛЕНО: теперь это метод класса)
    void apply() const {
        if (shader) {
            shader->use();
            shader->setUniform("ambient", ambient);
            shader->setUniform("diffuse", diffuse);
            shader->setUniform("specular", specular);
            shader->setUniform("shininess", shininess);
            shader->setUniform("hasTexture", hasTextureFlag);

            if (hasTextureFlag) {
                // Здесь код для текстуры
            }
        }
    }
};