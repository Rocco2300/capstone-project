#include "Shader.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    auto vertexShader = loadFile(vertexPath);
    auto fragmentShader = loadFile(fragmentPath);

    auto vertexId = compileShader(vertexShader.c_str(), GL_VERTEX_SHADER);
    auto fragmentId = compileShader(fragmentShader.c_str(), GL_FRAGMENT_SHADER);

    linkProgram(vertexId, fragmentId);
    glDeleteShader(vertexId);
    glDeleteShader(fragmentId);
}

uint32 Shader::getId() { return m_id; }

void Shader::use() { glUseProgram(m_id); }

void Shader::setUniform(const std::string& name, glm::mat4 mat) {
    auto location = glGetUniformLocation(m_id, name.c_str());
    if (location == -1) {
        std::cerr << "Uniform " << name << " not found in program.\n";
        return;
    }

    glProgramUniformMatrix4fv(m_id, location, 1, GL_FALSE, glm::value_ptr(mat));
}

std::string Shader::loadFile(const std::string& path) {
    std::ifstream in(path);
    if (in.fail()) {
        std::cout << "Error: Cannot open file " << path << '\n';
        return "";
    }

    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

uint32 Shader::compileShader(const char* shaderCode, uint32 shaderType) {
    uint32 shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderCode, nullptr);
    glCompileShader(shaderId);

    int success{};
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        char error[512];
        glGetShaderInfoLog(shaderId, 512, nullptr, error);

        auto shaderTypeStr = (shaderType == GL_VERTEX_SHADER) ? "vertex" : "fragment";
        std::cerr << "Error: Compilation of " << shaderTypeStr << " failed.\n";
        std::cerr << error << '\n';

        return 0;
    }

    return shaderId;
}

void Shader::linkProgram(uint32 vertexShaderId, uint32 fragmentShaderId) {
    m_id = glCreateProgram();
    glAttachShader(m_id, vertexShaderId);
    glAttachShader(m_id, fragmentShaderId);
    glLinkProgram(m_id);

    int success{};
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        char error[512];
        glGetProgramInfoLog(m_id, 512, nullptr, error);

        std::cerr << "Error: Linking failed.\n";
        std::cerr << error << '\n';
    }
}