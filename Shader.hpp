#pragma once
#include <string>
#include <unordered_map>
#include <GL/glew.h>

// Forward declarations
struct Color;
struct vec2;
struct vec3;
struct vec4;
struct mat4;

class Shader {
private:
    unsigned int programId;
    std::unordered_map<std::string, int> uniformLocations;

    std::string readFile(const std::string& filepath);
    unsigned int compileShader(const std::string& source, unsigned int type);
    void checkCompileErrors(unsigned int shader, const std::string& type);
    int getUniformLocation(const std::string& name);

public:
    Shader();
    ~Shader();

    // Загрузка из файлов
    bool loadFromFile(const std::string& vertexPath, const std::string& fragmentPath);

    // Загрузка из строк (для встроенных шейдеров)
    bool loadFromString(const std::string& vertexSource, const std::string& fragmentSource);

    // Использование
    void use() const;
    void unuse() const;

    // Uniform-переменные
    void setUniform(const std::string& name, int value);
    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, const vec2& value);
    void setUniform(const std::string& name, const vec3& value);
    void setUniform(const std::string& name, const vec4& value);
    void setUniform(const std::string& name, const Color& value);
    void setUniform(const std::string& name, const mat4& value);

    unsigned int getProgram() const { return programId; }
};