#include "Shader.hpp"
#include "color.hpp"
#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader() : programId(0) {}

Shader::~Shader() {
    if (programId != 0) {
        glDeleteProgram(programId);
    }
}

std::string Shader::readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }

    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

unsigned int Shader::compileShader(const std::string& source, unsigned int type) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    checkCompileErrors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
    return shader;
}

void Shader::checkCompileErrors(unsigned int shader, const std::string& type) {
    int success;
    char infoLog[1024];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cout << "Shader compilation error (" << type << "):\n" << infoLog << std::endl;
    }
}

int Shader::getUniformLocation(const std::string& name) {
    auto it = uniformLocations.find(name);
    if (it != uniformLocations.end()) {
        return it->second;
    }

    int location = glGetUniformLocation(programId, name.c_str());
    if (location == -1) {
        std::cout << "Warning: Uniform '" << name << "' not found in shader!" << std::endl;
    }
    uniformLocations[name] = location;
    return location;
}

bool Shader::loadFromString(const std::string& vertexSource, const std::string& fragmentSource) {
    std::cout << "Compiling vertex shader..." << std::endl;
    unsigned int vertex = compileShader(vertexSource, GL_VERTEX_SHADER);

    std::cout << "Compiling fragment shader..." << std::endl;
    unsigned int fragment = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    std::cout << "Linking program..." << std::endl;
    programId = glCreateProgram();
    glAttachShader(programId, vertex);
    glAttachShader(programId, fragment);
    glLinkProgram(programId);

    int success;
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(programId, 1024, nullptr, infoLog);
        std::cout << "Program linking error:\n" << infoLog << std::endl;
        return false;
    }

    std::cout << "Shader loaded successfully! Program ID: " << programId << std::endl;

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return true;
}

// Загрузка шейдера из файлов
bool Shader::loadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexSource   = readFile(vertexPath);
    std::string fragmentSource = readFile(fragmentPath);

    if (vertexSource.empty() || fragmentSource.empty()) {
        std::cout << "Failed to read shader files: " << vertexPath
                  << " and/or " << fragmentPath << std::endl;
        return false;
    }

    return loadFromString(vertexSource, fragmentSource);
}

void Shader::use() const {
    glUseProgram(programId);
}

void Shader::unuse() const {
    glUseProgram(0);
}

// ===== ИСПРАВЛЕННЫЕ UNIFORM-ФУНКЦИИ =====
void Shader::setUniform(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniform(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setUniform(const std::string& name, const vec2& value) {
    glUniform2f(getUniformLocation(name), value.x, value.y);
}

void Shader::setUniform(const std::string& name, const vec3& value) {
    glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

void Shader::setUniform(const std::string& name, const vec4& value) {
    glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::setUniform(const std::string& name, const Color& value) {
    glUniform4f(getUniformLocation(name), value.r, value.g, value.b, value.a);
}

void Shader::setUniform(const std::string& name, const mat4& value) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value.ptr());
}